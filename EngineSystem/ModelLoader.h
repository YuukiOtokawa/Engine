// ========================================================
//
// Assimp用モデル読み込みクラス[ModelLoader.h]
// 
//									Date:20250527
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// マクロ定義
//==========================================================================

#define NOMINMAX

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include <unordered_map>

#include "cimport.h"
#include "scene.h"
#include "postprocess.h"
#include "matrix4x4.h"


#pragma comment(lib, "assimp/assimp-vc143-mt.lib")

#include "object.h"
#include <d3d11.h>

//==========================================================================
// クラス定義
//==========================================================================

class ModelLoader
{
private:
	// Assimpインポート用モデル構造体
	struct MODEL
	{
		const aiScene* AiScene = nullptr;

		ID3D11Buffer** VertexBuffer;
		ID3D11Buffer** IndexBuffer;

		std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;
	};

public:
	/// @brief オブジェクトにモデルデータをファイルから読み込みます。
	/// @param object モデルデータを読み込む対象のObject型ポインタ。
	/// @param filename 読み込むモデルファイルのパスを示す文字列。
	void LoadModel(Object* object, const char* filename);
};

