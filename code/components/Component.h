// ========================================================
//
// コンポーネントクラス[Component.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "FrameWork.h"
#include "Object.h"

#include "EngineMetaFile.h"

#include "ComponentFactory.h"

using namespace ComponentTag;

//==========================================================================
// クラス定義
//==========================================================================

class Component : public EngineMetaFile
{
protected:

    // trueならコンポーネントはアクティブであることを示すフラグ
	bool isActive = true;

    bool isStarted = false;

	// コンポーネントのタグ
	Tag tag = NoComponent;

public:
    // コンポーネントの所有者オブジェクト
	Object* owner = nullptr;

	// コンストラクタ
    Component();

    virtual ~Component() = default; // 仮想デストラクタを定義

    virtual const char* GetComponentName() const = 0;
	
	/// @brief オブジェクトの所有者を設定し、タグを初期化します。
	/// @param obj 新しい所有者となるObject型のポインタ。
	virtual void SetOwner(Object* obj) { 
		owner = obj;
	}

	/// @brief タグを初期化する純粋仮想関数です。
	virtual void InitializeTag() = 0;

    virtual void Start() {}

    virtual void OnDestroy() {}

    virtual void Update() {
        if (!isStarted) {
            Start();
            isStarted = true;
        }
        UpdateComponent();
    }

	/// @brief コンポーネントを更新する純粋仮想関数です。
	virtual void UpdateComponent() = 0;

	/// @brief 図形やオブジェクトを描画するための仮想関数です。
	virtual void Draw() {}

	/// @brief GUI を描画するための仮想関数です。
	virtual void DrawGUI() {}

    void ExportFile() override {
        CSVExporter::ExportString("&");// コンポーネントの識別子をエクスポート
        CSVExporter::ExportInt(static_cast<int>(GetTag()));
        ExportComponent();
    }

    virtual void ExportComponent() {
        // デフォルトの実装は何もしない
    }

    void AddExportList() override {
        CSVExporter::AddExportList(this);
    }

    ComponentTag::Tag GetTag() const {
        return tag;
    }

    virtual void OnCollisionEnter(Object* object) {}
    virtual void OnCollisionStay(Object* object) {}
    virtual void OnCollisionExit(Object* object) {}
};

