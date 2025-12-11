// ========================================================
//
// プロジェクトウィンドウ[ProjectWindow.cpp]
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#include "ProjectWindow.h"
#include "EngineConsole.h"
#include "MainEngine.h"
#include "imgui.h"
#include "Prefab.h"

#include <Windows.h>
#include <shellapi.h>
#include <fstream>
#include <algorithm>

#include "DirectXTex.h"

ProjectWindow::ProjectWindow()
    : m_showContextMenu(false)
    , m_showRenameDialog(false)
    , m_showCreateScriptDialog(false)
    , m_showCreateFolderDialog(false)
    , m_showDeleteConfirmDialog(false)
{
    memset(m_renameBuffer, 0, sizeof(m_renameBuffer));
    memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
}

ProjectWindow::~ProjectWindow()
{
    Finalize();
}

void ProjectWindow::Initialize(const std::string& rootPath, const std::string& vcxprojPath)
{
    m_rootPath = rootPath;

    // scriptsフォルダが存在しない場合は作成
    if (!fs::exists(m_rootPath)) {
        fs::create_directories(m_rootPath);
        EngineConsole::Log("ProjectWindow: scriptsフォルダを作成しました");
    }

    // vcxprojパスが指定されている場合、VCProjectManagerを初期化
    if (!vcxprojPath.empty()) {
        if (!m_vcProjectManager.Initialize(vcxprojPath)) {
            EngineConsole::LogWarning("ProjectWindow: VCProjectManagerの初期化に失敗しました");
        }
    }

    // アイコンテクスチャを読み込む
    LoadIconTextures();

    // 初期表示パスをルートに設定
    m_currentPath = m_rootPath;

    Refresh();
}

void ProjectWindow::Finalize()
{
    ReleaseIconTextures();
    m_rootEntry.children.clear();
}

void ProjectWindow::Refresh()
{
    m_rootEntry.name = "scripts";
    m_rootEntry.fullPath = m_rootPath;
    m_rootEntry.isDirectory = true;
    m_rootEntry.isExpanded = true;
    m_rootEntry.children.clear();

    ScanDirectory(m_rootPath, m_rootEntry);
    
    // 現在のパスが有効でなくなった場合はルートに戻る
    if (m_currentPath != m_rootPath) {
        FileEntry* currentEntry = GetEntryByPath(m_currentPath);
        if (!currentEntry) {
            m_currentPath = m_rootPath;
            EngineConsole::LogWarning("ProjectWindow: 現在のパスが無効なためルートに戻りました");
        }
    }
}

void ProjectWindow::ScanDirectory(const std::string& path, FileEntry& entry)
{
    try {
        // パスが存在しない場合はスキップ
        if (!fs::exists(path)) {
            return;
        }

        // ディレクトリでない場合はスキップ
        if (!fs::is_directory(path)) {
            return;
        }

        std::vector<FileEntry> directories;
        std::vector<FileEntry> files;

        // skip_permission_deniedオプションでアクセス拒否をスキップ
        std::error_code ec;
        for (const auto& item : fs::directory_iterator(path, fs::directory_options::skip_permission_denied, ec)) {
            try {
                FileEntry child;
                child.name = item.path().filename().string();
                child.fullPath = item.path().string();
                child.isDirectory = item.is_directory(ec);
                child.isExpanded = false;

                if (ec) continue; // エラーがあればスキップ

                if (child.isDirectory) {
                    ScanDirectory(child.fullPath, child);
                    directories.push_back(child);
                }
                else {
                    // ソースファイルとPrefabを表示（.cpp, .h, .hpp, .c, .prefab）
                    std::string ext = item.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if (ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".prefab") {
                        files.push_back(child);
                    }
                }
            }
            catch (...) {
                // 個別のファイルでエラーが発生しても続行
                continue;
            }
        }

        // 名前順にソート
        std::sort(directories.begin(), directories.end(), [](const FileEntry& a, const FileEntry& b) {
            return a.name < b.name;
        });
        std::sort(files.begin(), files.end(), [](const FileEntry& a, const FileEntry& b) {
            return a.name < b.name;
        });

        // フォルダを先に、その後ファイル
        entry.children.clear();
        entry.children.insert(entry.children.end(), directories.begin(), directories.end());
        entry.children.insert(entry.children.end(), files.begin(), files.end());
    }
    catch (const fs::filesystem_error& e) {
        EngineConsole::LogError("ProjectWindow: ディレクトリスキャンエラー: %s (path: %s)", e.what(), path.c_str());
    }
    catch (const std::exception& e) {
        EngineConsole::LogError("ProjectWindow: エラー: %s (path: %s)", e.what(), path.c_str());
    }
}

void ProjectWindow::Render()
{
	ImGui::Begin("Project");
    // ツールバー
    // 戻るボタン
    bool canGoBack = (m_currentPath != m_rootPath);
    if (!canGoBack) ImGui::BeginDisabled();
    if (ImGui::Button("<")) {
        // 親フォルダに移動
        m_currentPath = fs::path(m_currentPath).parent_path().string();
    }
    if (!canGoBack) ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        Refresh();
    }
    ImGui::SameLine();
    if (ImGui::Button("+ Script")) {
        m_showCreateScriptDialog = true;
        m_createTargetDir = m_currentPath;
        memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
        strcpy_s(m_createNameBuffer, "NewScript");
    }
    ImGui::SameLine();
    if (ImGui::Button("+ Folder")) {
        m_showCreateFolderDialog = true;
        m_createTargetDir = m_currentPath;
        memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
        strcpy_s(m_createNameBuffer, "NewFolder");
    }

    // パンくずリスト
    DrawBreadcrumb();

    ImGui::Separator();

    // グリッドビュー
    ImGui::BeginChild("FileGrid", ImVec2(0, 0), true);

    DrawGridView();

    // 空白部分の右クリックメニュー
    if (ImGui::BeginPopupContextWindow("ProjectEmptyContext", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("New Script")) {
            m_showCreateScriptDialog = true;
            m_createTargetDir = m_currentPath;
            memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
            strcpy_s(m_createNameBuffer, "NewScript");
        }
        if (ImGui::MenuItem("New Folder")) {
            m_showCreateFolderDialog = true;
            m_createTargetDir = m_currentPath;
            memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
            strcpy_s(m_createNameBuffer, "NewFolder");
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Refresh")) {
            Refresh();
        }
        if (ImGui::MenuItem("Open in Explorer")) {
            ShellExecuteA(NULL, "explore", m_currentPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::EndPopup();
    }

    ImGui::EndChild();

    // ダイアログの描画
    DrawContextMenu();

    // 新規スクリプト作成ダイアログ
    if (m_showCreateScriptDialog) {
        ImGui::OpenPopup("Create New Script");
    }
    if (ImGui::BeginPopupModal("Create New Script", &m_showCreateScriptDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Script Name:");
        ImGui::InputText("##ScriptName", m_createNameBuffer, sizeof(m_createNameBuffer));
        ImGui::Separator();
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            CreateNewScript(m_createTargetDir);
            m_showCreateScriptDialog = false;
            Refresh();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showCreateScriptDialog = false;
        }
        ImGui::EndPopup();
    }

    // 新規フォルダ作成ダイアログ
    if (m_showCreateFolderDialog) {
        ImGui::OpenPopup("Create New Folder");
    }
    if (ImGui::BeginPopupModal("Create New Folder", &m_showCreateFolderDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Folder Name:");
        ImGui::InputText("##FolderName", m_createNameBuffer, sizeof(m_createNameBuffer));
        ImGui::Separator();
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            CreateNewFolder(m_createTargetDir);
            m_showCreateFolderDialog = false;
            Refresh();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showCreateFolderDialog = false;
        }
        ImGui::EndPopup();
    }

    // リネームダイアログ
    if (m_showRenameDialog) {
        ImGui::OpenPopup("Rename");
    }
    if (ImGui::BeginPopupModal("Rename", &m_showRenameDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New Name:");
        ImGui::InputText("##NewName", m_renameBuffer, sizeof(m_renameBuffer));
        ImGui::Separator();
        if (ImGui::Button("Rename", ImVec2(120, 0))) {
            RenameEntry(m_renameTargetPath, m_renameBuffer);
            m_showRenameDialog = false;
            Refresh();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showRenameDialog = false;
        }
        ImGui::EndPopup();
    }

    // 削除確認ダイアログ
    if (m_showDeleteConfirmDialog) {
        ImGui::OpenPopup("Confirm Delete");
    }
    if (ImGui::BeginPopupModal("Confirm Delete", &m_showDeleteConfirmDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete:");
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%s", fs::path(m_deleteTargetPath).filename().string().c_str());
        ImGui::Separator();
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            DeleteEntry(m_deleteTargetPath);
            m_showDeleteConfirmDialog = false;
            m_selectedPath.clear();
            Refresh();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showDeleteConfirmDialog = false;
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void ProjectWindow::DrawFileEntry(FileEntry& entry)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!entry.isDirectory) {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    if (m_selectedPath == entry.fullPath) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // ルートは常に展開
    if (entry.fullPath == m_rootPath) {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    // アイコン表示
    DrawIcon(entry.isDirectory, fs::path(entry.name).extension().string());
    ImGui::SameLine();

    bool nodeOpen = ImGui::TreeNodeEx(entry.name.c_str(), flags);

    // クリック処理
    if (ImGui::IsItemClicked()) {
        m_selectedPath = entry.fullPath;
    }

    // ダブルクリックでファイルを開く
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !entry.isDirectory) {
        std::string ext = fs::path(entry.fullPath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".prefab") {
            // Prefabの場合は編集モードに入る
            Editor::GetInstance()->LoadPrefabForEdit(entry.fullPath);
        }
        else {
            // その他のファイルは外部エディタで開く
            OpenInExternalEditor(entry.fullPath);
        }
    }

    // 右クリックメニュー
    if (ImGui::BeginPopupContextItem()) {
        if (entry.isDirectory) {
            if (ImGui::MenuItem("New Script")) {
                m_showCreateScriptDialog = true;
                m_createTargetDir = entry.fullPath;
                memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
                strcpy_s(m_createNameBuffer, "NewScript");
            }
            if (ImGui::MenuItem("New Folder")) {
                m_showCreateFolderDialog = true;
                m_createTargetDir = entry.fullPath;
                memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
                strcpy_s(m_createNameBuffer, "NewFolder");
            }
            ImGui::Separator();
        }

        if (ImGui::MenuItem("Open in Explorer")) {
            if (entry.isDirectory) {
                ShellExecuteA(NULL, "explore", entry.fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
            else {
                std::string parentDir = fs::path(entry.fullPath).parent_path().string();
                ShellExecuteA(NULL, "explore", parentDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }

        if (!entry.isDirectory) {
            // Prefabファイルの場合
            std::string ext = fs::path(entry.fullPath).extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".prefab") {
                if (ImGui::MenuItem("Edit Prefab")) {
                    Editor::GetInstance()->LoadPrefabForEdit(entry.fullPath);
                }
                ImGui::Separator();
            }

            if (ImGui::MenuItem("Open in Editor")) {
                OpenInExternalEditor(entry.fullPath);
            }
        }

        ImGui::Separator();

        if (entry.fullPath != m_rootPath) {  // ルートフォルダは削除/リネーム不可
            if (ImGui::MenuItem("Rename")) {
                m_showRenameDialog = true;
                m_renameTargetPath = entry.fullPath;
                memset(m_renameBuffer, 0, sizeof(m_renameBuffer));
                std::string nameOnly = entry.isDirectory ? entry.name : fs::path(entry.name).stem().string();
                strcpy_s(m_renameBuffer, nameOnly.c_str());
            }

            if (ImGui::MenuItem("Delete")) {
                m_showDeleteConfirmDialog = true;
                m_deleteTargetPath = entry.fullPath;
            }
        }

        ImGui::EndPopup();
    }

    // 子エントリの描画
    if (entry.isDirectory && nodeOpen) {
        for (auto& child : entry.children) {
            DrawFileEntry(child);
        }
        ImGui::TreePop();
    }
}

void ProjectWindow::DrawContextMenu()
{
    // 必要に応じて追加のコンテキストメニュー処理
}

void ProjectWindow::CreateNewScript(const std::string& directory)
{
    std::string scriptName = m_createNameBuffer;
    if (scriptName.empty()) {
        EngineConsole::LogError("ProjectWindow: スクリプト名が空です");
        return;
    }

    // ヘッダーファイルの作成
    std::string headerPath = directory + "\\" + scriptName + ".h";
    std::string cppPath = directory + "\\" + scriptName + ".cpp";

    // 既に存在する場合はエラー
    if (fs::exists(headerPath) || fs::exists(cppPath)) {
        EngineConsole::LogError("ProjectWindow: 同名のファイルが既に存在します");
        return;
    }

    // ヘッダーファイルのテンプレート
    std::ofstream headerFile(headerPath);
    if (headerFile.is_open()) {
        headerFile << "#pragma once\n\n";
        headerFile << "#include \"Script.h\"\n\n";
        headerFile << "class " << scriptName << " : public Script\n";
        headerFile << "{\n";
        headerFile << "public:\n";
        headerFile << "    void Start() override;\n";
        headerFile << "    void Update() override;\n";
        headerFile << "};\n";
        headerFile.close();
    }

    // cppファイルのテンプレート
    std::ofstream cppFile(cppPath);
    if (cppFile.is_open()) {
        cppFile << "#include \"" << scriptName << ".h\"\n\n";
        cppFile << "REGISTER_SCRIPT(" << scriptName << ")\n\n";
        cppFile << "void " << scriptName << "::Start()\n";
        cppFile << "{\n";
        cppFile << "    // Initialize your script here\n";
        cppFile << "}\n\n";
        cppFile << "void " << scriptName << "::Update()\n";
        cppFile << "{\n";
        cppFile << "    // Update your script here\n";
        cppFile << "}\n";
        cppFile.close();
    }

    EngineConsole::Log("ProjectWindow: 新しいスクリプトを作成しました: %s", scriptName.c_str());

    // Visual Studioプロジェクトに追加
    if (!m_vcProjectManager.GetProjectPath().empty()) {
        // ファイルをプロジェクトに追加
        if (m_vcProjectManager.AddSourceFiles(cppPath, headerPath)) {
            EngineConsole::Log("ProjectWindow: Visual Studioプロジェクトにファイルを追加しました");
        } else {
            EngineConsole::LogWarning("ProjectWindow: Visual Studioプロジェクトへのファイル追加に失敗しました");
        }

        // ディレクトリをインクルードパスに追加（scriptsディレクトリ以外のサブディレクトリの場合）
        if (directory != m_rootPath) {
            if (m_vcProjectManager.AddIncludeDirectory(directory)) {
                EngineConsole::Log("ProjectWindow: Visual Studioプロジェクトにインクルードディレクトリを追加しました");
            } else {
                EngineConsole::LogWarning("ProjectWindow: インクルードディレクトリの追加に失敗しました");
            }
        }
    }
}

void ProjectWindow::CreateNewFolder(const std::string& directory)
{
    std::string folderName = m_createNameBuffer;
    if (folderName.empty()) {
        EngineConsole::LogError("ProjectWindow: フォルダ名が空です");
        return;
    }

    std::string folderPath = directory + "\\" + folderName;

    if (fs::exists(folderPath)) {
        EngineConsole::LogError("ProjectWindow: 同名のフォルダが既に存在します");
        return;
    }

    try {
        fs::create_directory(folderPath);
        EngineConsole::Log("ProjectWindow: 新しいフォルダを作成しました: %s", folderName.c_str());
    }
    catch (const fs::filesystem_error& e) {
        EngineConsole::LogError("ProjectWindow: フォルダ作成エラー: %s", e.what());
    }
}

void ProjectWindow::DeleteEntry(const std::string& path)
{
    try {
        if (fs::is_directory(path)) {
            fs::remove_all(path);
            EngineConsole::Log("ProjectWindow: フォルダを削除しました: %s", path.c_str());
        }
        else {
            // .h と .cpp を両方削除
            std::string stem = fs::path(path).stem().string();
            std::string dir = fs::path(path).parent_path().string();

            std::string headerPath = dir + "\\" + stem + ".h";
            std::string cppPath = dir + "\\" + stem + ".cpp";

            if (fs::exists(path)) {
                fs::remove(path);
            }

            // 対応するファイルも削除するか確認（ヘッダーを消したらcppも消す、など）
            std::string ext = fs::path(path).extension().string();
            if (ext == ".h" || ext == ".hpp") {
                if (fs::exists(cppPath)) {
                    fs::remove(cppPath);
                }
            }
            else if (ext == ".cpp" || ext == ".c") {
                if (fs::exists(headerPath)) {
                    fs::remove(headerPath);
                }
            }

            EngineConsole::Log("ProjectWindow: ファイルを削除しました: %s", path.c_str());
        }
    }
    catch (const fs::filesystem_error& e) {
        EngineConsole::LogError("ProjectWindow: 削除エラー: %s", e.what());
    }
}

void ProjectWindow::RenameEntry(const std::string& oldPath, const std::string& newName)
{
    try {
        fs::path oldFsPath(oldPath);
        std::string dir = oldFsPath.parent_path().string();
        bool isDir = fs::is_directory(oldPath);

        if (isDir) {
            std::string newPath = dir + "\\" + newName;
            fs::rename(oldPath, newPath);
            EngineConsole::Log("ProjectWindow: フォルダをリネームしました: %s -> %s", oldFsPath.filename().string().c_str(), newName.c_str());
        }
        else {
            // ファイルの場合は拡張子を保持
            std::string ext = oldFsPath.extension().string();
            std::string stem = oldFsPath.stem().string();
            std::string newPath = dir + "\\" + newName + ext;

            // 対応するヘッダー/cppも同時にリネーム
            std::string oldHeaderPath = dir + "\\" + stem + ".h";
            std::string oldCppPath = dir + "\\" + stem + ".cpp";
            std::string newHeaderPath = dir + "\\" + newName + ".h";
            std::string newCppPath = dir + "\\" + newName + ".cpp";

            if (fs::exists(oldHeaderPath)) {
                fs::rename(oldHeaderPath, newHeaderPath);
            }
            if (fs::exists(oldCppPath)) {
                fs::rename(oldCppPath, newCppPath);
            }

            EngineConsole::Log("ProjectWindow: ファイルをリネームしました: %s -> %s", stem.c_str(), newName.c_str());
        }
    }
    catch (const fs::filesystem_error& e) {
        EngineConsole::LogError("ProjectWindow: リネームエラー: %s", e.what());
    }
}

void ProjectWindow::OpenInExternalEditor(const std::string& path)
{
    // デフォルトの関連付けられたプログラムで開く
    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void ProjectWindow::DrawIcon(bool isDirectory, const std::string& extension)
{
    ID3D11ShaderResourceView* icon = nullptr;

    if (isDirectory) {
        icon = m_pFolderIcon;
    }
    else {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".h" || ext == ".hpp") {
            icon = m_pHeaderIcon;
        }
        else if (ext == ".cpp" || ext == ".c") {
            icon = m_pCppIcon;
        }
    }

    // アイコンを描画
    if (icon) {
        ImGui::Image((ImTextureID)icon, ImVec2(m_iconSize, m_iconSize));
    }
    else {
        // フォールバック: テキストアイコン
        if (isDirectory) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[D]");
        }
        else {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[F]");
        }
    }
}

void ProjectWindow::LoadIconTextures()
{
    auto device = MainEngine::GetInstance()->GetRenderCore()->GetDevice();

    TexMetadata metadata;
	ScratchImage image;

    // フォルダアイコン
    LoadFromWICFile(
        L"EngineResource/Folder.png",
        WIC_FLAGS_NONE, 
        &metadata,
        image
    );
    CreateShaderResourceView(
        device,
        image.GetImages(),
        image.GetImageCount(),
        metadata,
        &m_pFolderIcon
	);

    // C++アイコン
    LoadFromWICFile(
        L"EngineResource/CPlusPlus.png",
        WIC_FLAGS_NONE,
        &metadata,
        image
    );
    CreateShaderResourceView(
        device,
        image.GetImages(),
        image.GetImageCount(),
        metadata,
        &m_pCppIcon
    );

    // ヘッダーアイコン
    LoadFromWICFile(
        L"EngineResource/Header.png",
        WIC_FLAGS_NONE,
        &metadata,
        image
    );
    CreateShaderResourceView(
        device,
        image.GetImages(),
        image.GetImageCount(),
        metadata,
        &m_pHeaderIcon
	);

    // プレハブアイコン
    LoadFromWICFile(
        L"EngineResource/Prefab.png",
        WIC_FLAGS_NONE,
        &metadata,
        image
    );
    CreateShaderResourceView(
        device,
        image.GetImages(),
        image.GetImageCount(),
        metadata,
		&m_pPrefabIcon
	);

    EngineConsole::Log("ProjectWindow: アイコンテクスチャを読み込みました");
}

void ProjectWindow::ReleaseIconTextures()
{
    if (m_pFolderIcon) {
        m_pFolderIcon->Release();
        m_pFolderIcon = nullptr;
    }
    if (m_pCppIcon) {
        m_pCppIcon->Release();
        m_pCppIcon = nullptr;
    }
    if (m_pHeaderIcon) {
        m_pHeaderIcon->Release();
        m_pHeaderIcon = nullptr;
    }
    if (m_pPrefabIcon) {
        m_pPrefabIcon->Release();
        m_pPrefabIcon = nullptr;
    }
}

void ProjectWindow::DrawBreadcrumb()
{
    // パスを分割してパンくずリストを作成
    fs::path currentFsPath(m_currentPath);
    fs::path rootFsPath(m_rootPath);

    std::vector<std::pair<std::string, std::string>> pathParts;

    // scriptsからの相対パスを構築
    fs::path relativePath = fs::relative(currentFsPath, rootFsPath.parent_path());

    fs::path buildPath = rootFsPath.parent_path();
    for (const auto& part : relativePath) {
        buildPath /= part;
        pathParts.push_back({ part.string(), buildPath.string() });
    }

    // パンくずリストを描画
    for (size_t i = 0; i < pathParts.size(); ++i) {
        if (i > 0) {
            ImGui::SameLine();
            ImGui::TextDisabled(">");
            ImGui::SameLine();
        }

        // 最後の要素は現在地なのでボタンではなくテキスト
        if (i == pathParts.size() - 1) {
            ImGui::Text("%s", pathParts[i].first.c_str());
        }
        else {
            if (ImGui::SmallButton(pathParts[i].first.c_str())) {
                m_currentPath = pathParts[i].second;
            }
        }
    }
}

void ProjectWindow::DrawGridView()
{
    // 現在のフォルダのエントリを取得
    FileEntry* currentEntry = GetEntryByPath(m_currentPath);
    if (!currentEntry) {
        ImGui::Text("Folder not found");
        return;
    }

    // ウィンドウの幅を取得
    float windowWidth = ImGui::GetContentRegionAvail().x;
    int itemsPerRow = (int)((windowWidth + m_gridSpacing) / (m_gridItemWidth + m_gridSpacing));
    if (itemsPerRow < 1) itemsPerRow = 1;

    int itemIndex = 0;
    for (auto& child : currentEntry->children) {
        // グリッドアイテムを描画
        if (DrawGridItem(child)) {
            // ダブルクリックでフォルダに入る or ファイルを開く
            if (child.isDirectory) {
                m_currentPath = child.fullPath;
            }
            else {
                // Prefabファイルの場合は編集モードに入る
                std::string ext = fs::path(child.fullPath).extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".prefab") {
                    Editor::GetInstance()->LoadPrefabForEdit(child.fullPath);
                }
                else {
                    OpenInExternalEditor(child.fullPath);
                }
            }
        }

        itemIndex++;

        // 行の最後でなければSameLine
        if (itemIndex % itemsPerRow != 0) {
            ImGui::SameLine(0, m_gridSpacing);
        }
    }
}

bool ProjectWindow::DrawGridItem(const FileEntry& entry)
{
    bool doubleClicked = false;

    ImGui::PushID(entry.fullPath.c_str());

    // アイテム全体のサイズでグループ化
    ImGui::BeginGroup();

    // 選択状態の背景
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    bool isSelected = (m_selectedPath == entry.fullPath);

    if (isSelected) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + m_gridItemWidth, cursorPos.y + m_gridItemHeight),
            IM_COL32(60, 100, 180, 100),
            4.0f
        );
    }

    // アイコンを中央に配置
    float iconOffsetX = (m_gridItemWidth - m_iconSize) / 2.0f;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + iconOffsetX);

    // アイコン取得
    ID3D11ShaderResourceView* icon = nullptr;
    if (entry.isDirectory) {
        icon = m_pFolderIcon;
    }
    else {
        std::string ext = fs::path(entry.name).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".h" || ext == ".hpp") {
            icon = m_pHeaderIcon;
        }
        else if (ext == ".cpp" || ext == ".c") {
            icon = m_pCppIcon;
        }
        else if (ext == ".prefab") {
            icon = m_pPrefabIcon;
        }
    }

    // アイコン描画
    if (icon) {
        ImGui::Image((ImTextureID)icon, ImVec2(m_iconSize, m_iconSize));
    }
    else {
        // フォールバック
        ImGui::Dummy(ImVec2(m_iconSize, m_iconSize));
    }

    // ファイル名を中央揃えで表示（折り返しあり）
    std::string displayName = entry.name;

    // 長すぎる名前は省略
    float textWidth = ImGui::CalcTextSize(displayName.c_str()).x;
    if (textWidth > m_gridItemWidth - 4) {
        // 省略表示
        while (displayName.length() > 3 && ImGui::CalcTextSize((displayName + "...").c_str()).x > m_gridItemWidth - 4) {
            displayName.pop_back();
        }
        displayName += "...";
    }

    // テキストを中央揃え
    textWidth = ImGui::CalcTextSize(displayName.c_str()).x;
    float textOffsetX = (m_gridItemWidth - textWidth) / 2.0f;
    if (textOffsetX < 0) textOffsetX = 0;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffsetX);
    ImGui::TextWrapped("%s", displayName.c_str());

    ImGui::EndGroup();

    // インタラクション
    ImVec2 itemMin = cursorPos;
    ImVec2 itemMax = ImVec2(cursorPos.x + m_gridItemWidth, cursorPos.y + m_gridItemHeight);

    // 透明なボタンでクリック検出
    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton("##item", ImVec2(m_gridItemWidth, m_gridItemHeight));

    if (ImGui::IsItemClicked()) {
        m_selectedPath = entry.fullPath;
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        doubleClicked = true;
    }

    // Prefabファイルのドラッグ&ドロップソース
    if (!entry.isDirectory) {
        std::string ext = fs::path(entry.fullPath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".prefab") {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                // ペイロードとしてファイルパスを設定
                ImGui::SetDragDropPayload("PREFAB_FILE", entry.fullPath.c_str(), entry.fullPath.size() + 1);
                // ドラッグ中の表示
                ImGui::Text("Prefab: %s", entry.name.c_str());
                ImGui::EndDragDropSource();
            }
        }
    }

    // 右クリックメニュー
    if (ImGui::BeginPopupContextItem()) {
        if (entry.isDirectory) {
            if (ImGui::MenuItem("Open")) {
                m_currentPath = entry.fullPath;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("New Script")) {
                m_showCreateScriptDialog = true;
                m_createTargetDir = entry.fullPath;
                memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
                strcpy_s(m_createNameBuffer, "NewScript");
            }
            if (ImGui::MenuItem("New Folder")) {
                m_showCreateFolderDialog = true;
                m_createTargetDir = entry.fullPath;
                memset(m_createNameBuffer, 0, sizeof(m_createNameBuffer));
                strcpy_s(m_createNameBuffer, "NewFolder");
            }
            ImGui::Separator();
        }
        else {
            // Prefabファイルの場合
            std::string ext = fs::path(entry.fullPath).extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".prefab") {
                if (ImGui::MenuItem("Edit Prefab")) {
                    Editor::GetInstance()->LoadPrefabForEdit(entry.fullPath);
                }
                if (ImGui::MenuItem("Instantiate")) {
                    Prefab::Import(entry.fullPath);
                }
                ImGui::Separator();
            }
            if (ImGui::MenuItem("Open in Editor")) {
                OpenInExternalEditor(entry.fullPath);
            }
        }

        if (ImGui::MenuItem("Open in Explorer")) {
            if (entry.isDirectory) {
                ShellExecuteA(NULL, "explore", entry.fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
            else {
                std::string parentDir = fs::path(entry.fullPath).parent_path().string();
                ShellExecuteA(NULL, "explore", parentDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Rename")) {
            m_showRenameDialog = true;
            m_renameTargetPath = entry.fullPath;
            memset(m_renameBuffer, 0, sizeof(m_renameBuffer));
            std::string nameOnly = entry.isDirectory ? entry.name : fs::path(entry.name).stem().string();
            strcpy_s(m_renameBuffer, nameOnly.c_str());
        }

        if (ImGui::MenuItem("Delete")) {
            m_showDeleteConfirmDialog = true;
            m_deleteTargetPath = entry.fullPath;
        }

        ImGui::EndPopup();
    }

    // ホバー時のハイライト
    if (ImGui::IsItemHovered()) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRect(
            itemMin,
            itemMax,
            IM_COL32(100, 150, 255, 200),
            4.0f,
            0,
            2.0f
        );
    }

    ImGui::PopID();

    return doubleClicked;
}

FileEntry* ProjectWindow::GetEntryByPath(const std::string& path)
{
    if (path == m_rootPath || path == m_rootEntry.fullPath) {
        return &m_rootEntry;
    }
    return GetEntryByPathRecursive(m_rootEntry, path);
}

FileEntry* ProjectWindow::GetEntryByPathRecursive(FileEntry& entry, const std::string& path)
{
    for (auto& child : entry.children) {
        if (child.fullPath == path) {
            return &child;
        }
        if (child.isDirectory) {
            FileEntry* found = GetEntryByPathRecursive(child, path);
            if (found) return found;
        }
    }
    return nullptr;
}
