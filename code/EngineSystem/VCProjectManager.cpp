// ========================================================
//
// Visual Studio プロジェクトファイル管理[VCProjectManager.cpp]
//
//									Date:20250526
//									Author:Yuuki Otokawa
// ========================================================

#include "VCProjectManager.h"
#include "EngineConsole.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

VCProjectManager::VCProjectManager()
{
}

VCProjectManager::~VCProjectManager()
{
}

bool VCProjectManager::Initialize(const std::string& vcxprojPath)
{
    m_vcxprojPath = vcxprojPath;

    // プロジェクトファイルの存在確認
    if (!fs::exists(m_vcxprojPath)) {
        EngineConsole::LogError("VCProjectManager: プロジェクトファイルが見つかりません: %s", m_vcxprojPath.c_str());
        return false;
    }

    // プロジェクトルートディレクトリを取得
    m_projectRoot = fs::path(m_vcxprojPath).parent_path().string();

    EngineConsole::Log("VCProjectManager: 初期化しました - %s", m_vcxprojPath.c_str());
    return true;
}

bool VCProjectManager::AddSourceFiles(const std::string& cppPath, const std::string& headerPath)
{
    // プロジェクトファイルを読み込む
    if (!LoadProject()) {
        return false;
    }

    // 相対パスに変換
    std::string relativeCppPath = GetRelativePath(cppPath);
    std::string relativeHeaderPath = GetRelativePath(headerPath);

    bool modified = false;

    // .cppファイルを追加
    if (!IsFileInProject(m_projectContent, relativeCppPath)) {
        if (AddClCompileEntry(m_projectContent, relativeCppPath)) {
            EngineConsole::Log("VCProjectManager: .cppファイルをプロジェクトに追加しました: %s", relativeCppPath.c_str());
            modified = true;
        }
    }

    // .hファイルを追加
    if (!IsFileInProject(m_projectContent, relativeHeaderPath)) {
        if (AddClIncludeEntry(m_projectContent, relativeHeaderPath)) {
            EngineConsole::Log("VCProjectManager: .hファイルをプロジェクトに追加しました: %s", relativeHeaderPath.c_str());
            modified = true;
        }
    }

    // 変更があれば保存
    if (modified) {
        return SaveProject();
    }

    return true;
}

bool VCProjectManager::RemoveSourceFiles(const std::string& filePath)
{
    // プロジェクトファイルを読み込む
    if (!LoadProject()) {
        return false;
    }

    // 相対パスに変換
    std::string relativePath = GetRelativePath(filePath);

    // 拡張子を取得
    fs::path path(filePath);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    bool modified = false;

    // ファイルの種類に応じて削除
    if (ext == ".cpp" || ext == ".c") {
        if (RemoveClCompileEntry(m_projectContent, relativePath)) {
            EngineConsole::Log("VCProjectManager: .cppファイルをプロジェクトから削除しました: %s", relativePath.c_str());
            modified = true;
        }
    }
    else if (ext == ".h" || ext == ".hpp") {
        if (RemoveClIncludeEntry(m_projectContent, relativePath)) {
            EngineConsole::Log("VCProjectManager: .hファイルをプロジェクトから削除しました: %s", relativePath.c_str());
            modified = true;
        }
    }

    // 変更があれば保存
    if (modified) {
        return SaveProject();
    }

    return false;
}

bool VCProjectManager::AddIncludeDirectory(const std::string& dirPath)
{
    // プロジェクトファイルを読み込む
    if (!LoadProject()) {
        return false;
    }

    // 相対パスに変換
    std::string relativeDir = GetRelativePath(dirPath);

    // 既に含まれているか確認
    if (IsDirectoryInIncludePaths(m_projectContent, relativeDir)) {
        EngineConsole::Log("VCProjectManager: ディレクトリは既にインクルードパスに含まれています: %s", relativeDir.c_str());
        return true;
    }

    // インクルードディレクトリを追加
    if (UpdateIncludeDirectories(m_projectContent, relativeDir)) {
        EngineConsole::Log("VCProjectManager: インクルードディレクトリを追加しました: %s", relativeDir.c_str());
        return SaveProject();
    }

    return false;
}

bool VCProjectManager::LoadProject()
{
    std::ifstream file(m_vcxprojPath);
    if (!file.is_open()) {
        EngineConsole::LogError("VCProjectManager: プロジェクトファイルを開けませんでした: %s", m_vcxprojPath.c_str());
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_projectContent = buffer.str();
    file.close();

    return true;
}

bool VCProjectManager::SaveProject()
{
    std::ofstream file(m_vcxprojPath);
    if (!file.is_open()) {
        EngineConsole::LogError("VCProjectManager: プロジェクトファイルを保存できませんでした: %s", m_vcxprojPath.c_str());
        return false;
    }

    file << m_projectContent;
    file.close();

    EngineConsole::Log("VCProjectManager: プロジェクトファイルを保存しました");
    return true;
}

std::string VCProjectManager::GetRelativePath(const std::string& absolutePath) const
{
    try {
        fs::path absPath = fs::absolute(absolutePath);
        fs::path projRoot = fs::absolute(m_projectRoot);
        fs::path relativePath = fs::relative(absPath, projRoot);

        // Windowsのパス区切り文字に変換
        std::string result = relativePath.string();
        std::replace(result.begin(), result.end(), '/', '\\');

        return result;
    }
    catch (const std::exception& e) {
        EngineConsole::LogError("VCProjectManager: 相対パス変換エラー: %s", e.what());
        return absolutePath;
    }
}

bool VCProjectManager::UpdateIncludeDirectories(std::string& content, const std::string& relativeDir)
{
    // <AdditionalIncludeDirectories> タグを検索
    size_t pos = content.find("<AdditionalIncludeDirectories>");

    if (pos == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: AdditionalIncludeDirectoriesタグが見つかりませんでした");
        return false;
    }

    // タグの終了位置を検索
    size_t endTagPos = content.find("</AdditionalIncludeDirectories>", pos);
    if (endTagPos == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: AdditionalIncludeDirectoriesの終了タグが見つかりませんでした");
        return false;
    }

    // 既存のインクルードディレクトリリストを取得
    size_t contentStart = pos + std::string("<AdditionalIncludeDirectories>").length();
    std::string existingDirs = content.substr(contentStart, endTagPos - contentStart);

    // 新しいディレクトリを追加（セミコロンで区切る）
    std::string newDirs = relativeDir + ";" + existingDirs;

    // 内容を置換
    content.replace(contentStart, endTagPos - contentStart, newDirs);

    return true;
}

bool VCProjectManager::AddClCompileEntry(std::string& content, const std::string& relativePath)
{
    // </ItemGroup> を検索（ClCompileセクションの最後）
    // まず、ClCompile要素を含むItemGroupを見つける
    size_t clCompilePos = content.find("<ClCompile Include=");
    if (clCompilePos == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClCompileセクションが見つかりませんでした");
        return false;
    }

    // そのItemGroupの終了タグを検索
    size_t itemGroupStart = content.rfind("<ItemGroup>", clCompilePos);
    size_t itemGroupEnd = content.find("</ItemGroup>", clCompilePos);

    if (itemGroupEnd == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClCompile ItemGroupの終了タグが見つかりませんでした");
        return false;
    }

    // 最後のClCompile要素を探す
    size_t lastClCompile = clCompilePos;
    size_t searchPos = clCompilePos;
    while (true) {
        searchPos = content.find("<ClCompile Include=", searchPos + 1);
        if (searchPos == std::string::npos || searchPos > itemGroupEnd) {
            break;
        }
        lastClCompile = searchPos;
    }

    // 最後のClCompile要素の終了位置を見つける
    size_t insertPos = content.find("/>", lastClCompile);
    if (insertPos == std::string::npos) {
        // 閉じタグ形式の場合
        insertPos = content.find("</ClCompile>", lastClCompile);
        if (insertPos != std::string::npos) {
            insertPos = content.find("\n", insertPos) + 1;
        }
    } else {
        insertPos = content.find("\n", insertPos) + 1;
    }

    if (insertPos == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClCompile挿入位置が見つかりませんでした");
        return false;
    }

    // 新しいエントリを作成
    std::string newEntry = "    <ClCompile Include=\"" + relativePath + "\" />\n";

    // 挿入
    content.insert(insertPos, newEntry);

    return true;
}

bool VCProjectManager::AddClIncludeEntry(std::string& content, const std::string& relativePath)
{
    // ClInclude要素を含むItemGroupを見つける
    size_t clIncludePos = content.find("<ClInclude Include=");
    if (clIncludePos == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClIncludeセクションが見つかりませんでした");
        return false;
    }

    // そのItemGroupの終了タグを検索
    size_t itemGroupEnd = content.find("</ItemGroup>", clIncludePos);

    if (itemGroupEnd == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClInclude ItemGroupの終了タグが見つかりませんでした");
        return false;
    }

    // 最後のClInclude要素を探す
    size_t lastClInclude = clIncludePos;
    size_t searchPos = clIncludePos;
    while (true) {
        searchPos = content.find("<ClInclude Include=", searchPos + 1);
        if (searchPos == std::string::npos || searchPos > itemGroupEnd) {
            break;
        }
        lastClInclude = searchPos;
    }

    // 最後のClInclude要素の終了位置を見つける
    size_t insertPos = content.find("/>", lastClInclude);
    if (insertPos == std::string::npos) {
        // 閉じタグ形式の場合
        insertPos = content.find("</ClInclude>", lastClInclude);
        if (insertPos != std::string::npos) {
            insertPos = content.find("\n", insertPos) + 1;
        }
    } else {
        insertPos = content.find("\n", insertPos) + 1;
    }

    if (insertPos == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClInclude挿入位置が見つかりませんでした");
        return false;
    }

    // 新しいエントリを作成
    std::string newEntry = "    <ClInclude Include=\"" + relativePath + "\" />\n";

    // 挿入
    content.insert(insertPos, newEntry);

    return true;
}

bool VCProjectManager::RemoveClCompileEntry(std::string& content, const std::string& relativePath)
{
    // <ClCompile Include="relativePath" /> または <ClCompile Include="relativePath"></ClCompile> を検索
    std::string searchPattern = "<ClCompile Include=\"" + relativePath + "\"";
    size_t pos = content.find(searchPattern);

    if (pos == std::string::npos) {
        EngineConsole::LogWarning("VCProjectManager: ClCompileエントリが見つかりませんでした: %s", relativePath.c_str());
        return false;
    }

    // エントリの開始位置（行の先頭）を見つける
    size_t lineStart = content.rfind('\n', pos);
    if (lineStart == std::string::npos) {
        lineStart = 0;
    } else {
        lineStart++; // 改行の次の文字から
    }

    // エントリの終了位置を見つける
    size_t lineEnd = content.find("/>", pos);
    if (lineEnd == std::string::npos) {
        // 閉じタグ形式の場合
        lineEnd = content.find("</ClCompile>", pos);
        if (lineEnd != std::string::npos) {
            lineEnd += std::string("</ClCompile>").length();
        }
    } else {
        lineEnd += 2; // "/>" の長さ
    }

    if (lineEnd == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClCompileエントリの終了位置が見つかりませんでした");
        return false;
    }

    // 次の改行まで含めて削除
    size_t nextNewline = content.find('\n', lineEnd);
    if (nextNewline != std::string::npos) {
        lineEnd = nextNewline + 1;
    }

    // エントリを削除
    content.erase(lineStart, lineEnd - lineStart);

    return true;
}

bool VCProjectManager::RemoveClIncludeEntry(std::string& content, const std::string& relativePath)
{
    // <ClInclude Include="relativePath" /> または <ClInclude Include="relativePath"></ClInclude> を検索
    std::string searchPattern = "<ClInclude Include=\"" + relativePath + "\"";
    size_t pos = content.find(searchPattern);

    if (pos == std::string::npos) {
        EngineConsole::LogWarning("VCProjectManager: ClIncludeエントリが見つかりませんでした: %s", relativePath.c_str());
        return false;
    }

    // エントリの開始位置（行の先頭）を見つける
    size_t lineStart = content.rfind('\n', pos);
    if (lineStart == std::string::npos) {
        lineStart = 0;
    } else {
        lineStart++; // 改行の次の文字から
    }

    // エントリの終了位置を見つける
    size_t lineEnd = content.find("/>", pos);
    if (lineEnd == std::string::npos) {
        // 閉じタグ形式の場合
        lineEnd = content.find("</ClInclude>", pos);
        if (lineEnd != std::string::npos) {
            lineEnd += std::string("</ClInclude>").length();
        }
    } else {
        lineEnd += 2; // "/>" の長さ
    }

    if (lineEnd == std::string::npos) {
        EngineConsole::LogError("VCProjectManager: ClIncludeエントリの終了位置が見つかりませんでした");
        return false;
    }

    // 次の改行まで含めて削除
    size_t nextNewline = content.find('\n', lineEnd);
    if (nextNewline != std::string::npos) {
        lineEnd = nextNewline + 1;
    }

    // エントリを削除
    content.erase(lineStart, lineEnd - lineStart);

    return true;
}

bool VCProjectManager::IsFileInProject(const std::string& content, const std::string& relativePath) const
{
    // Include="relativePath" を検索
    std::string searchStr = "Include=\"" + relativePath + "\"";
    return content.find(searchStr) != std::string::npos;
}

bool VCProjectManager::IsDirectoryInIncludePaths(const std::string& content, const std::string& relativeDir) const
{
    // AdditionalIncludeDirectoriesセクション内を検索
    size_t pos = content.find("<AdditionalIncludeDirectories>");
    if (pos == std::string::npos) {
        return false;
    }

    size_t endPos = content.find("</AdditionalIncludeDirectories>", pos);
    if (endPos == std::string::npos) {
        return false;
    }

    // そのセクション内の文字列を取得
    std::string includeSection = content.substr(pos, endPos - pos);

    // ディレクトリが含まれているか確認
    return includeSection.find(relativeDir) != std::string::npos;
}
