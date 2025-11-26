// ========================================================
//
// Visual Studio プロジェクトファイル管理[VCProjectManager.h]
// .vcxprojファイルにファイルやインクルードディレクトリを追加
//
//									Date:20250526
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "EngineAPI.h"
#include <string>
#include <set>

class OTOKAWA_API VCProjectManager
{
public:
    VCProjectManager();
    ~VCProjectManager();

    /// @brief 初期化
    /// @param vcxprojPath vcxprojファイルのパス
    /// @return 成功した場合true
    bool Initialize(const std::string& vcxprojPath);

    /// @brief ソースファイルとヘッダーファイルをプロジェクトに追加
    /// @param cppPath .cppファイルのパス
    /// @param headerPath .hファイルのパス
    /// @return 成功した場合true
    bool AddSourceFiles(const std::string& cppPath, const std::string& headerPath);

    /// @brief インクルードディレクトリをプロジェクトに追加（まだ存在しない場合）
    /// @param dirPath 追加するディレクトリパス
    /// @return 成功した場合true
    bool AddIncludeDirectory(const std::string& dirPath);

    /// @brief vcxprojファイルのパスを取得
    const std::string& GetProjectPath() const { return m_vcxprojPath; }

private:
    /// @brief vcxprojファイルを読み込む
    bool LoadProject();

    /// @brief vcxprojファイルに保存
    bool SaveProject();

    /// @brief 相対パスに変換（プロジェクトルートからの相対パス）
    std::string GetRelativePath(const std::string& absolutePath) const;

    /// @brief インクルードディレクトリのセクションを更新
    bool UpdateIncludeDirectories(std::string& content, const std::string& relativeDir);

    /// @brief ClCompileセクションに.cppファイルを追加
    bool AddClCompileEntry(std::string& content, const std::string& relativePath);

    /// @brief ClIncludeセクションに.hファイルを追加
    bool AddClIncludeEntry(std::string& content, const std::string& relativePath);

    /// @brief 既にファイルがプロジェクトに含まれているか確認
    bool IsFileInProject(const std::string& content, const std::string& relativePath) const;

    /// @brief 既にディレクトリがインクルードパスに含まれているか確認
    bool IsDirectoryInIncludePaths(const std::string& content, const std::string& relativeDir) const;

private:
    std::string m_vcxprojPath;      // vcxprojファイルのパス
    std::string m_projectRoot;      // プロジェクトのルートディレクトリ
    std::string m_projectContent;   // vcxprojファイルの内容
};
