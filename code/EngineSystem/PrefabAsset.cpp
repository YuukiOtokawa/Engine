// ========================================================
//
// Prefabアセット参照クラス[PrefabAsset.cpp]
//
//									Date:20250526
//									Author:Yuuki Otokawa
// ========================================================

#include "PrefabAsset.h"
#include "Prefab.h"
#include "EngineConsole.h"
#include <filesystem>

namespace fs = std::filesystem;

PrefabAsset::PrefabAsset()
    : m_prefabPath("")
{
}

PrefabAsset::~PrefabAsset()
{
}

void PrefabAsset::SetPath(const std::string& path)
{
    // パスが空の場合はそのまま設定
    if (path.empty()) {
        m_prefabPath = "";
        return;
    }

    // ファイルが存在するか確認
    if (!fs::exists(path)) {
        EngineConsole::LogWarning("PrefabAsset: ファイルが存在しません: %s", path.c_str());
        m_prefabPath = path; // パスは保持するが警告を出す
        return;
    }

    // 拡張子が.prefabか確認
    std::string ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".prefab") {
        EngineConsole::LogWarning("PrefabAsset: .prefab ファイルではありません: %s", path.c_str());
    }

    m_prefabPath = path;
    EngineConsole::Log("PrefabAsset: Prefabを設定しました: %s", path.c_str());
}

Object* PrefabAsset::Instantiate() const
{
    if (!IsValid()) {
        EngineConsole::LogError("PrefabAsset: Prefabパスが設定されていません");
        return nullptr;
    }

    // Prefabからオブジェクトをインスタンス化
    Object* instance = Prefab::Import(m_prefabPath);
    if (!instance) {
        EngineConsole::LogError("PrefabAsset: Prefabのインスタンス化に失敗しました: %s", m_prefabPath.c_str());
        return nullptr;
    }

    EngineConsole::Log("PrefabAsset: Prefabをインスタンス化しました: %s", m_prefabPath.c_str());
    return instance;
}

bool PrefabAsset::SelectFromDialog()
{
    // Prefabインポートダイアログを開く
    Object* tempObject = Prefab::ImportWithDialog();

    // ダイアログがキャンセルされた場合
    if (!tempObject) {
        return false;
    }

    // 一時オブジェクトを削除（パスだけ取得したい）
    // TODO: より良い方法でパスを取得する実装が必要
    // 現状はImportWithDialogの実装を確認して、
    // パスだけ取得できるメソッドを用意する必要がある

    delete tempObject;
    return true;
}

std::string PrefabAsset::GetFileName() const
{
    if (m_prefabPath.empty()) {
        return "None";
    }

    return fs::path(m_prefabPath).filename().string();
}
