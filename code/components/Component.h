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
#include "SceneExporter.h"

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

    virtual void OnUpdate() {
        if (!isStarted) {
            Start();
            isStarted = true;
        }
        Update();
    }

	/// @brief コンポーネントを更新する純粋仮想関数です。
	virtual void Update() = 0;


	/// @brief GUI を描画するための仮想関数です。
	virtual void DrawGUI() {}

    void RightClickMenu() override;

    void Destroy() {
        if (owner) {
            owner->DeleteComponent(this);
        }
    }

    void ExportFile(YAML::Emitter& out) override {
        out << YAML::Key << "tag" << YAML::Value << static_cast<int>(GetTag());
        ExportComponent(out);
    }

    virtual void ExportComponent(YAML::Emitter& out) {
        // デフォルトの実装は何もしない
    }

    void AddExportList() override {
        SceneExporter::AddExportList(this);
    }

    ComponentTag::Tag GetTag() const {
        return tag;
    }

    virtual void OnCollisionEnter(Object* object) {}
    virtual void OnCollisionStay(Object* object) {}
    virtual void OnCollisionExit(Object* object) {}
};

