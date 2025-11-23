// ========================================================
//
// プレハブクラス[Prefab.h]
// オブジェクトとコンポーネントをYAMLファイルに保存/読み込み
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "EngineAPI.h"
#include "EngineMetaFile.h"
#include <string>
#include <list>

class Object;

class OTOKAWA_API Prefab
{
public:
    /// @brief オブジェクトをPrefabファイルとして保存
    /// @param object 保存するオブジェクト
    /// @param filePath 保存先のファイルパス（.prefab）
    /// @return 成功した場合true
    static bool Export(Object* object, const std::string& filePath);

    /// @brief Prefabファイルからオブジェクトを生成
    /// @param filePath 読み込むPrefabファイルのパス
    /// @return 生成されたオブジェクト（失敗時はnullptr）
    static Object* Import(const std::string& filePath);

    /// @brief ファイルダイアログを開いてオブジェクトをエクスポート
    /// @param object 保存するオブジェクト
    /// @return 成功した場合true
    static bool ExportWithDialog(Object* object);

    /// @brief ファイルダイアログを開いてPrefabをインポート
    /// @return 生成されたオブジェクト（失敗時はnullptr）
    static Object* ImportWithDialog();

private:
    /// @brief オブジェクトとそのコンポーネントをエクスポートリストに追加
    static void CollectExportData(Object* object, std::list<EngineMetaFile*>& exportList);

    /// @brief ClassIDからオブジェクトを生成
    static EngineMetaFile* CreateFromClassID(ClassID classID);

    /// @brief エクスポート用ファイルダイアログを開く
    static std::string OpenExportDialog(const std::string& defaultName);

    /// @brief インポート用ファイルダイアログを開く
    static std::string OpenImportDialog();
};
