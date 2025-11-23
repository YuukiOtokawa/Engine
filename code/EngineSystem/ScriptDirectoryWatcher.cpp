// ========================================================
//
// スクリプトディレクトリ監視システム[ScriptDirectoryWatcher.cpp]
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#include "ScriptDirectoryWatcher.h"
#include "../Aurora/RuntimeObjectSystem/IRuntimeObjectSystem.h"
#include "EngineConsole.h"

#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

ScriptDirectoryWatcher::ScriptDirectoryWatcher()
    : m_pRuntimeObjectSystem(nullptr)
    , m_bRunning(false)
    , m_hStopEvent(NULL)
{
}

ScriptDirectoryWatcher::~ScriptDirectoryWatcher()
{
    Finalize();
}

bool ScriptDirectoryWatcher::Initialize(const std::string& watchPath, IRuntimeObjectSystem* pRuntimeObjectSystem)
{
    if (!pRuntimeObjectSystem) {
        EngineConsole::LogError("ScriptDirectoryWatcher: RuntimeObjectSystemがnullです");
        return false;
    }

    m_watchPath = watchPath;
    m_pRuntimeObjectSystem = pRuntimeObjectSystem;

    // パスが存在するか確認
    if (!fs::exists(m_watchPath)) {
        EngineConsole::LogWarning("ScriptDirectoryWatcher: 監視対象パスが存在しません: %s", m_watchPath.c_str());
        // フォルダが存在しない場合は作成
        fs::create_directories(m_watchPath);
        EngineConsole::Log("ScriptDirectoryWatcher: scriptsフォルダを作成しました: %s", m_watchPath.c_str());
    }

    // 既存ファイルをスキャンして登録
    ScanAndRegisterExistingFiles(m_watchPath);

    // 停止イベントを作成
    m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hStopEvent) {
        EngineConsole::LogError("ScriptDirectoryWatcher: 停止イベントの作成に失敗しました");
        return false;
    }

    // 監視スレッドを開始
    m_bRunning = true;
    m_watchThread = std::thread(&ScriptDirectoryWatcher::WatchThreadProc, this);

    EngineConsole::Log("ScriptDirectoryWatcher: 監視を開始しました - %s (%zu個のファイルを登録)",
                       m_watchPath.c_str(), m_registeredFiles.size());

    return true;
}

void ScriptDirectoryWatcher::Finalize()
{
    // 監視スレッドを停止
    if (m_bRunning) {
        m_bRunning = false;
        if (m_hStopEvent) {
            SetEvent(m_hStopEvent);
        }
        if (m_watchThread.joinable()) {
            m_watchThread.join();
        }
    }

    if (m_hStopEvent) {
        CloseHandle(m_hStopEvent);
        m_hStopEvent = NULL;
    }

    m_registeredFiles.clear();
    m_registeredDirs.clear();
    m_pRuntimeObjectSystem = nullptr;
}

void ScriptDirectoryWatcher::Update()
{
    // 保留中の変更を処理
    std::vector<FileChangeEvent> changes;
    {
        std::lock_guard<std::mutex> lock(m_changeMutex);
        changes = std::move(m_pendingChanges);
        m_pendingChanges.clear();
    }

    for (const auto& change : changes) {
        ProcessFileChange(change.filename, change.action);
    }
}

void ScriptDirectoryWatcher::ScanAndRegisterExistingFiles(const std::string& directory)
{
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                if (IsSourceFile(filepath)) {
                    RegisterFile(filepath);
                }
            }
            else if (entry.is_directory()) {
                RegisterIncludeDirectory(entry.path().string());
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        EngineConsole::LogError("ScriptDirectoryWatcher: ディレクトリスキャンエラー: %s", e.what());
    }
}

bool ScriptDirectoryWatcher::IsSourceFile(const std::string& filename) const
{
    std::string ext = fs::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".c");
}

void ScriptDirectoryWatcher::RegisterFile(const std::string& filepath)
{
    // 正規化されたパスを取得
    std::string normalizedPath = fs::absolute(filepath).string();

    // 既に登録済みの場合はスキップ
    if (m_registeredFiles.count(normalizedPath) > 0) {
        return;
    }

    // RCC++にファイルを登録（.cppファイルのみ）
    std::string ext = fs::path(normalizedPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".cpp" || ext == ".c") {
        m_pRuntimeObjectSystem->AddToRuntimeFileList(normalizedPath.c_str());
        EngineConsole::Log("ScriptDirectoryWatcher: ソースファイルを登録: %s", normalizedPath.c_str());
    }

    m_registeredFiles.insert(normalizedPath);
}

void ScriptDirectoryWatcher::UnregisterFile(const std::string& filepath)
{
    std::string normalizedPath = fs::absolute(filepath).string();

    auto it = m_registeredFiles.find(normalizedPath);
    if (it == m_registeredFiles.end()) {
        return;
    }

    // RCC++から登録解除（.cppファイルのみ）
    std::string ext = fs::path(normalizedPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".cpp" || ext == ".c") {
        m_pRuntimeObjectSystem->RemoveFromRuntimeFileList(normalizedPath.c_str());
        EngineConsole::Log("ScriptDirectoryWatcher: ソースファイルを登録解除: %s", normalizedPath.c_str());
    }

    m_registeredFiles.erase(it);
}

void ScriptDirectoryWatcher::RegisterIncludeDirectory(const std::string& dirPath)
{
    std::string normalizedPath = fs::absolute(dirPath).string();

    if (m_registeredDirs.count(normalizedPath) > 0) {
        return;
    }

    m_pRuntimeObjectSystem->AddIncludeDir(normalizedPath.c_str());
    m_registeredDirs.insert(normalizedPath);
    EngineConsole::Log("ScriptDirectoryWatcher: インクルードディレクトリを登録: %s", normalizedPath.c_str());
}

void ScriptDirectoryWatcher::WatchThreadProc()
{
    // ディレクトリ変更通知用ハンドルを取得
    HANDLE hDir = CreateFileA(
        m_watchPath.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        EngineConsole::LogError("ScriptDirectoryWatcher: ディレクトリハンドルの取得に失敗しました");
        return;
    }

    // オーバーラップI/O用の構造体
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    char buffer[4096];
    HANDLE waitHandles[2] = { overlapped.hEvent, m_hStopEvent };

    while (m_bRunning) {
        // 変更通知を要求
        DWORD bytesReturned = 0;
        BOOL success = ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            TRUE,  // サブディレクトリも監視
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_CREATION,
            &bytesReturned,
            &overlapped,
            NULL
        );

        if (!success) {
            DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING) {
                EngineConsole::LogError("ScriptDirectoryWatcher: ReadDirectoryChangesWが失敗しました: %d", error);
                break;
            }
        }

        // イベントを待機（停止イベントまたは変更通知）
        DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {
            // 変更通知を受信
            if (!GetOverlappedResult(hDir, &overlapped, &bytesReturned, FALSE)) {
                continue;
            }

            // 変更内容を解析
            FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

            do {
                // ファイル名をワイド文字からマルチバイトに変換
                int nameLen = pNotify->FileNameLength / sizeof(WCHAR);
                std::wstring wfilename(pNotify->FileName, nameLen);

                int mbLen = WideCharToMultiByte(CP_UTF8, 0, wfilename.c_str(), -1, NULL, 0, NULL, NULL);
                std::string filename(mbLen, 0);
                WideCharToMultiByte(CP_UTF8, 0, wfilename.c_str(), -1, &filename[0], mbLen, NULL, NULL);
                filename.resize(mbLen - 1);  // null終端を除去

                // フルパスを構築
                std::string fullPath = m_watchPath + "\\" + filename;

                // 変更イベントをキューに追加
                {
                    std::lock_guard<std::mutex> lock(m_changeMutex);
                    m_pendingChanges.push_back({ fullPath, pNotify->Action });
                }

                // 次のエントリへ
                if (pNotify->NextEntryOffset == 0) {
                    break;
                }
                pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                    reinterpret_cast<char*>(pNotify) + pNotify->NextEntryOffset
                );
            } while (true);

            ResetEvent(overlapped.hEvent);
        }
        else if (waitResult == WAIT_OBJECT_0 + 1) {
            // 停止イベント
            break;
        }
    }

    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}

void ScriptDirectoryWatcher::ProcessFileChange(const std::string& filename, DWORD action)
{
    switch (action) {
    case FILE_ACTION_ADDED:
    case FILE_ACTION_RENAMED_NEW_NAME:
        if (fs::is_regular_file(filename)) {
            if (IsSourceFile(filename)) {
                RegisterFile(filename);
            }
        }
        else if (fs::is_directory(filename)) {
            // 新しいディレクトリが追加された場合、その中の既存ファイルもスキャン
            RegisterIncludeDirectory(filename);
            ScanAndRegisterExistingFiles(filename);
        }
        break;

    case FILE_ACTION_REMOVED:
    case FILE_ACTION_RENAMED_OLD_NAME:
        // 削除されたファイル/ディレクトリの登録を解除
        if (IsSourceFile(filename)) {
            UnregisterFile(filename);
        }
        break;

    case FILE_ACTION_MODIFIED:
        // ファイル変更はRCC++のFileChangeNotifierが処理するので、ここでは何もしない
        break;
    }
}
