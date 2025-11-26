// ========================================================
//
// プロジェクトウィンドウ[ProjectWindow.h]
// scriptsフォルダの内容を表示・管理するウィンドウ
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "EngineAPI.h"
#include "VCProjectManager.h"
#include <string>
#include <vector>
#include <filesystem>
#include <d3d11.h>

namespace fs = std::filesystem;

// ファイル/フォルダエントリの構造体
struct FileEntry {
    std::string name;           // 表示名
    std::string fullPath;       // フルパス
    bool isDirectory;           // ディレクトリかどうか
    bool isExpanded;            // 展開状態（フォルダ用）
    std::vector<FileEntry> children; // 子エントリ
};

class OTOKAWA_API ProjectWindow
{
public:
    ProjectWindow();
    ~ProjectWindow();

    /// @brief 初期化
    /// @param rootPath scriptsフォルダのパス
    /// @param vcxprojPath vcxprojファイルのパス（オプション）
    void Initialize(const std::string& rootPath, const std::string& vcxprojPath = "");

    /// @brief 終了処理
    void Finalize();

    /// @brief フォルダ構造を再スキャン
    void Refresh();

    /// @brief ウィンドウの描画
    void Draw();

    /// @brief ルートパスを取得
    const std::string& GetRootPath() const { return m_rootPath; }

private:
    /// @brief ディレクトリを再帰的にスキャン
    void ScanDirectory(const std::string& path, FileEntry& entry);

    /// @brief ファイルエントリを再帰的に描画（ツリービュー用）
    void DrawFileEntry(FileEntry& entry);

    /// @brief グリッドビューでアイコンを描画
    void DrawGridView();

    /// @brief 単一アイテムをグリッド形式で描画
    bool DrawGridItem(const FileEntry& entry);

    /// @brief パンくずリスト（ナビゲーション）を描画
    void DrawBreadcrumb();

    /// @brief コンテキストメニューを描画
    void DrawContextMenu();

    /// @brief 新しいスクリプトファイルを作成
    void CreateNewScript(const std::string& directory);

    /// @brief 新しいフォルダを作成
    void CreateNewFolder(const std::string& directory);

    /// @brief ファイル/フォルダを削除
    void DeleteEntry(const std::string& path);

    /// @brief ファイル/フォルダをリネーム
    void RenameEntry(const std::string& oldPath, const std::string& newName);

    /// @brief 外部エディタでファイルを開く
    void OpenInExternalEditor(const std::string& path);

    /// @brief アイコン描画（ファイルタイプに応じたアイコン表示）
    void DrawIcon(bool isDirectory, const std::string& extension);

    /// @brief アイコンテクスチャを読み込む
    void LoadIconTextures();

    /// @brief アイコンテクスチャを解放
    void ReleaseIconTextures();

private:
    std::string m_rootPath;             // scriptsフォルダのパス
    FileEntry m_rootEntry;              // ルートエントリ

    std::string m_selectedPath;         // 選択されたファイル/フォルダのパス
    std::string m_contextMenuPath;      // コンテキストメニューの対象パス
    bool m_showContextMenu;             // コンテキストメニュー表示フラグ

    // リネームダイアログ用
    bool m_showRenameDialog;
    char m_renameBuffer[256];
    std::string m_renameTargetPath;

    // 新規作成ダイアログ用
    bool m_showCreateScriptDialog;
    bool m_showCreateFolderDialog;
    char m_createNameBuffer[256];
    std::string m_createTargetDir;

    // 削除確認ダイアログ用
    bool m_showDeleteConfirmDialog;
    std::string m_deleteTargetPath;

    // アイコンテクスチャ
    ID3D11ShaderResourceView* m_pFolderIcon = nullptr;
    ID3D11ShaderResourceView* m_pCppIcon = nullptr;
    ID3D11ShaderResourceView* m_pHeaderIcon = nullptr;
    ID3D11ShaderResourceView* m_pPrefabIcon = nullptr;

    // アイコンサイズ
    float m_iconSize = 64.0f;

    // グリッド表示設定
    float m_gridItemWidth = 80.0f;      // グリッドアイテムの幅
    float m_gridItemHeight = 100.0f;    // グリッドアイテムの高さ
    float m_gridSpacing = 10.0f;        // アイテム間のスペース

    // 現在表示中のフォルダパス
    std::string m_currentPath;

    /// @brief 指定パスのFileEntryを取得
    FileEntry* GetEntryByPath(const std::string& path);
    FileEntry* GetEntryByPathRecursive(FileEntry& entry, const std::string& path);

    // Visual Studioプロジェクト管理
    VCProjectManager m_vcProjectManager;
};
