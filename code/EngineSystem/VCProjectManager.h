// ========================================================
//
// Visual Studio プロジェクトファイル管理[VCProjectManager.h]
//
//									Date:20250526
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include <string>

class VCProjectManager
{
public:
    VCProjectManager();
    ~VCProjectManager();

    // 初期化
    bool Initialize(const std::string& vcxprojPath);

    // ソースファイルをプロジェクトに追加
    bool AddSourceFiles(const std::string& cppPath, const std::string& headerPath);

    // ソースファイルをプロジェクトから削除
    bool RemoveSourceFiles(const std::string& filePath);

    // インクルードディレクトリを追加
    bool AddIncludeDirectory(const std::string& dirPath);

    // プロジェクトパスを取得
    const std::string& GetProjectPath() const { return m_vcxprojPath; }

private:
    // プロジェクトファイルを読み込む
    bool LoadProject();

    // プロジェクトファイルを保存
    bool SaveProject();

    // 相対パスに変換
    std::string GetRelativePath(const std::string& absolutePath) const;

    // インクルードディレクトリを更新
    bool UpdateIncludeDirectories(std::string& content, const std::string& relativeDir);

    // ClCompileエントリを追加
    bool AddClCompileEntry(std::string& content, const std::string& relativePath);

    // ClIncludeエントリを追加
    bool AddClIncludeEntry(std::string& content, const std::string& relativePath);

    // ClCompileエントリを削除
    bool RemoveClCompileEntry(std::string& content, const std::string& relativePath);

    // ClIncludeエントリを削除
    bool RemoveClIncludeEntry(std::string& content, const std::string& relativePath);

    // ファイルがプロジェクトに含まれているか確認
    bool IsFileInProject(const std::string& content, const std::string& relativePath) const;

    // ディレクトリがインクルードパスに含まれているか確認
    bool IsDirectoryInIncludePaths(const std::string& content, const std::string& relativeDir) const;

private:
    std::string m_vcxprojPath;    // プロジェクトファイルのパス
    std::string m_projectRoot;     // プロジェクトルートディレクトリ
    std::string m_projectContent;  // プロジェクトファイルの内容
};
