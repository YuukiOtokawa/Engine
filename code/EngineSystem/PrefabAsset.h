// ========================================================
//
// Prefabアセット参照クラス[PrefabAsset.h]
// スクリプトからPrefabを参照・インスタンス化するためのクラス
//
//									Date:20250526
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "EngineAPI.h"
#include <string>

class Object;

/// @brief Prefabへの参照を保持し、インスタンス化を提供するクラス
class OTOKAWA_API PrefabAsset
{
public:
    PrefabAsset();
    ~PrefabAsset();

    /// @brief Prefabファイルパスを設定
    /// @param path Prefabファイルのパス
    void SetPath(const std::string& path);

    /// @brief Prefabファイルパスを取得
    /// @return Prefabファイルのパス
    const std::string& GetPath() const { return m_prefabPath; }

    /// @brief Prefabが有効か（パスが設定されているか）確認
    /// @return 有効な場合true
    bool IsValid() const { return !m_prefabPath.empty(); }

    /// @brief Prefabをシーン内にインスタンス化
    /// @return 生成されたオブジェクト（失敗時はnullptr）
    Object* Instantiate() const;

    /// @brief ファイル選択ダイアログを開いてPrefabを選択
    /// @return Prefabが選択された場合true
    bool SelectFromDialog();

    /// @brief Prefabファイル名を取得（パス付きでなく名前のみ）
    /// @return ファイル名
    std::string GetFileName() const;

private:
    std::string m_prefabPath; // Prefabファイルのパス
};
