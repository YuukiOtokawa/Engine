#include "ModelLoader.h"

#include "MainEngine.h"

#include "Component_Meshfilter.h"
#include "AssimpMeshRenderer.h"

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
				vertex[v].position = Vector4O(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);
				vertex[v].texcoord = Vector4O(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].color = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[v].normal = Vector4O(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
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

		model->Texture[aitexture->mFilename.data] = texture;
	}

	object->GetComponent<MeshFilter>()->SetVertexBuffer(*model->VertexBuffer, model->AiScene->mMeshes[0]->mNumVertices);
	object->GetComponent<MeshFilter>()->SetIndexBuffer(*model->IndexBuffer, model->AiScene->mMeshes[0]->mNumFaces * 3);
	object->GetComponent<AssimpMeshRenderer>()->SetMesh(model->Texture);
}
