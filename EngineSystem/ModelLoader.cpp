#define _CRT_SECURE_NO_WARNINGS
#include "ModelLoader.h"

#include "MainEngine.h"

#include "Component_Meshfilter.h"
#include "AssimpMeshRenderer.h"

#include "Component_Transform.h"

void ModelLoader::LoadModel(Object* object, const char* filename)
{
	ID3D11Device* device = MainEngine::GetInstance()->GetRenderer()->GetDevice();
	MODEL* model = new MODEL;


	const std::string modelPath(filename);

	model->AiScene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(model->AiScene);

	model->VertexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];//頂点データポインター
	model->IndexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];//インデックスデータポインター


	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX* vertex = new VERTEX[mesh->mNumVertices];//頂点数分の配列領域作成

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].position = Vector3O(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);
				vertex[v].texcoord = Vector2O(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].color = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[v].normal = Vector3O(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			device->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];//ポリゴン数数*3

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			device->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);

			delete[] index;
		}

	}

	//テクスチャ読み込み
	for (UINT i = 0; i < model->AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = model->AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture;
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		wchar_t* wc{};
		mbstowcs(wc, aitexture->mFilename.C_Str(), sizeof(wc));

		model->Texture[aitexture->mFilename.data].shader_resource_view = texture;
		model->Texture[aitexture->mFilename.data].width = (int)metadata.width;
		model->Texture[aitexture->mFilename.data].height = (int)metadata.height;
		if (wc != NULL)
			model->Texture[aitexture->mFilename.data].filename = wc;
	}
	if (model->AiScene->mNumMeshes != 1) {
		for (unsigned int i = 0; i < model->AiScene->mNumMeshes; i++) {
			Object* child = new Object();
			child->SetName(model->AiScene->mMeshes[i]->mName.C_Str());
			child->SetTag(GameObjectTagLayer::ObjectTag);
			child->AddComponent<Transform>();
			child->AddComponent<MeshFilter>();
			child->AddComponent<AssimpMeshRenderer>();
			
			aiString texture;
			model->AiScene->mMaterials[model->AiScene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

			child->GetComponent<MeshFilter>()->SetVertexBuffer(model->VertexBuffer[i], model->AiScene->mMeshes[i]->mNumVertices);
			child->GetComponent<MeshFilter>()->SetIndexBuffer(model->IndexBuffer[i], model->AiScene->mMeshes[i]->mNumFaces * 3);

			if (model->Texture.size() != 0) {
				int texID = MainEngine::GetInstance()->GetRenderer()->AddTexture(model->Texture[texture.data]);
				child->GetComponent<AssimpMeshRenderer>()->SetTexture(texID);
			}

			object->AddChild(child);
		}
	}
	else {
		aiString texture;
		model->AiScene->mMaterials[model->AiScene->mMeshes[0]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		object->GetComponent<MeshFilter>()->SetVertexBuffer(model->VertexBuffer[0], model->AiScene->mMeshes[0]->mNumVertices);
		object->GetComponent<MeshFilter>()->SetIndexBuffer(model->IndexBuffer[0], model->AiScene->mMeshes[0]->mNumFaces * 3);

		if (model->Texture.size() != 0) {
			int texID = MainEngine::GetInstance()->GetRenderer()->AddTexture(model->Texture[texture.data]);
			object->GetComponent<AssimpMeshRenderer>()->SetTexture(texID);
		}

	}
}
