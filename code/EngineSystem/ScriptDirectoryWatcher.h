// ========================================================
//
// スクリプトディレクトリ監視システム[ScriptDirectoryWatcher.h]
// scriptsフォルダの構造変化を監視し、RCC++に自動登録する
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "EngineAPI.h"
#include <string>
#include <vector>
#include <set>
#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>

struct IRuntimeObjectSystem;

class OTOKAWA_API ScriptDirectoryWatcher
{
public:
    ScriptDirectoryWatcher();
    ~ScriptDirectoryWatcher();

    /// @brief 監視を初期化して開始する
    /// @param watchPath 監視対象のディレクトリパス（scriptsフォルダ）
    /// @param pRuntimeObjectSystem RCC++のランタイムオブジェクトシステム
    /// @return 初期化成功時はtrue
    bool Initialize(const std::string& watchPath, IRuntimeObjectSystem* pRuntimeObjectSystem);

    /// @brief 監視を終了する
    void Finalize();

    /// @brief 毎フレーム呼び出して変更を処理する
    void Update();

    /// @brief 現在登録されているソースファイル数を取得
    size_t GetRegisteredFileCount() const { return m_registeredFiles.size(); }

private:
    /// @brief ディレクトリ内の既存ファイルをスキャンして登録
    void ScanAndRegisterExistingFiles(const std::string& directory);

    /// @brief ファイルがソースファイルかどうか判定（.cpp, .h）
    bool IsSourceFile(const std::string& filename) const;

    /// @brief ファイルをRCC++に登録
    void RegisterFile(const std::string& filepath);

    /// @brief ファイルをRCC++から登録解除
    void UnregisterFile(const std::string& filepath);

    /// @brief サブディレクトリをインクルードディレクトリとして登録
    void RegisterIncludeDirectory(const std::string& dirPath);

    /// @brief 監視スレッドのメイン処理
    void WatchThreadProc();

    /// @brief 変更イベントを処理
    void ProcessFileChange(const std::string& filename, DWORD action);

private:
    std::string m_watchPath;                        // 監視対象ディレクトリ
    IRuntimeObjectSystem* m_pRuntimeObjectSystem;   // RCC++システムへのポインタ

    std::set<std::string> m_registeredFiles;        // 登録済みファイルセット
    std::set<std::string> m_registeredDirs;         // 登録済みディレクトリセット

    // 監視スレッド関連
    std::thread m_watchThread;
    std::atomic<bool> m_bRunning;
    HANDLE m_hStopEvent;

    // 変更キュー（スレッド間通信用）
    struct FileChangeEvent {
        std::string filename;
        DWORD action;
    };
    std::vector<FileChangeEvent> m_pendingChanges;
    std::mutex m_changeMutex;
};
