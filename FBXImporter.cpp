#include "FBXImporter.h"

#include "MainEngine.h"

#include "Component_MeshFilter.h"
#include "ModelAnimator.h"

void FBXImporter::LoadFBX(const char* filename, Object* target)
{
	m_Model = new FBXMODEL;

	const std::string m_ModelPath(filename);

	m_Model->AiScene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	assert(m_Model->AiScene);

	m_Model->vertexBuffer = new ID3D11Buffer * [m_Model->AiScene->mNumMeshes];
	m_Model->indexBuffer = new ID3D11Buffer * [m_Model->AiScene->mNumMeshes];

	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_Model->AiScene->mNumMeshes];
	CreateBone(m_Model->AiScene->mRootNode);

	LoadVertexIndex(m_ModelPath);

	LoadTexture(m_ModelPath);

	LoadAnimation(m_ModelPath, "Base");

	CreateMaterial();

	MeshFilter* meshFilter = target->GetComponent<MeshFilter>();
	if (!meshFilter)
		meshFilter = target->AddComponent<MeshFilter>();

	meshFilter->SetMesh(m_VertexIndex->GetFileID());

	MeshRenderer* meshRenderer = target->GetComponent<MeshRenderer>();
	if (!meshRenderer)
		meshRenderer = target->AddComponent<MeshRenderer>();
	
	auto material = meshRenderer->GetMaterial();
	material->SetSubMaterials(m_SubMaterial);

	ModelAnimator* animator = target->GetComponent<ModelAnimator>();
	if (!animator)
		animator = target->AddComponent<ModelAnimator>();
	animator->SetAnimation(m_Animation);

}

void FBXImporter::LoadVertexIndex(std::string filename)
{
	for (unsigned int m = 0; m < m_Model->AiScene->mNumMeshes; m++) {
		aiMesh* mesh = m_Model->AiScene->mMeshes[m];

		// 頂点情報読み込み
		std::vector<VERTEX> vertices;

		{
			VERTEX vertex;
			for (unsigned int v = 0; v < mesh->mNumVertices; v++) {

				vertex.position = Vector3O(mesh->mVertices[v].x, -mesh->mVertices[v].z, mesh->mVertices[v].y);
				vertex.texcoord = Vector2O(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex.color = Vector4O(1.0f);
				vertex.normal = Vector3O(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);

				vertices.push_back(vertex);
			}
		}

		// インデックス情報読み込み
		std::vector<UINT> indices;

		{
			UINT index;
			for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
				const aiFace* face = &mesh->mFaces[f];
				assert(face->mNumIndices == 3);

				index = face->mIndices[0];
				indices.push_back(index);
				index = face->mIndices[1];
				indices.push_back(index);
				index = face->mIndices[2];
				indices.push_back(index);
			}
		}

		m_VertexIndex = new VertexIndex(filename.substr(filename.find_last_of('/') + 1), vertices, indices);
		m_VertexIndex->SetFilePath(filename);

		// 頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
			DEFORM_VERTEX deformVertex;
			deformVertex.position = mesh->mVertices[v];
			deformVertex.normal = mesh->mNormals[v];
			deformVertex.boneNum = 0;

			for (unsigned int b = 0; b < 4; b++) {
				deformVertex.boneName[b] = "";
				deformVertex.boneWeight[b] = 0.0f;
			}

			m_DeformVertex[m].push_back(deformVertex);
		}


		// ボーンデータ初期化
		for (unsigned int b = 0; b < mesh->mNumBones; b++) {
			aiBone* bone = mesh->mBones[b];

			m_Bone[bone->mName.C_Str()].offsetMatrix = bone->mOffsetMatrix;

			// 変形後頂点にボーンデータ格納
			for (unsigned int w = 0; w < bone->mNumWeights; w++) {
				aiVertexWeight weight = bone->mWeights[w];

				int num = m_DeformVertex[m][weight.mVertexId].boneNum;

				m_DeformVertex[m][weight.mVertexId].boneName[num] = bone->mName.C_Str();
				m_DeformVertex[m][weight.mVertexId].boneWeight[num] = weight.mWeight;
				m_DeformVertex[m][weight.mVertexId].boneNum++;

				assert(m_DeformVertex[m][weight.mVertexId].boneNum <= 4);
			}
		}
	}

}

void FBXImporter::LoadTexture(std::string filename)
{
	Renderer* renderer = MainEngine::GetInstance()->GetRenderer();

	for (UINT i = 0; i < m_Model->AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_Model->AiScene->mTextures[i];

		ID3D11ShaderResourceView* srv;

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(MainEngine::GetInstance()->GetRenderer()->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &srv);
		assert(srv);

		Texture* texture = new Texture();
		texture->shader_resource_view = srv;
		texture->filename = std::wstring(aitexture->mFilename.C_Str(), aitexture->mFilename.C_Str() + strlen(aitexture->mFilename.C_Str()));
		texture->height = metadata.height;
		texture->width = metadata.width;

		m_TextureFileID[aitexture->mFilename.data] = renderer->AddTexture(texture);
	}
}

void FBXImporter::LoadAnimation(std::string filename, const char* name) {
	
	m_Animation[name] = aiImportFile(filename.c_str(), aiProcess_ConvertToLeftHanded);
	assert(m_Animation[name]);
}

void FBXImporter::CreateMaterial()
{
	ID3D11Device* device = MainEngine::GetInstance()->GetRenderer()->GetDevice();
	// テクスチャ読み込み
	for (UINT i = 0; i < m_Model->AiScene->mNumMeshes; i++) {
		aiMaterial* aimaterial = m_Model->AiScene->mMaterials[m_Model->AiScene->mMeshes[i]->mMaterialIndex];
		
		aiString texture;
		aiColor3D diffuse;
		float opacity;
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);

		SUB_MATERIAL* subMaterial = new SUB_MATERIAL;

		if (texture == aiString("")) {
			subMaterial->textureID = -1;
			subMaterial->material.textureEnable = false;
		}
		else {
			subMaterial->material.textureEnable = true;
			subMaterial->textureID = m_TextureFileID[texture.data];
		}

		subMaterial->material.diffuse = Vector4O(diffuse.r, diffuse.g, diffuse.b, opacity);
		subMaterial->material.ambient = subMaterial->material.diffuse;

		m_SubMaterial.push_back(subMaterial);
	}
}

void FBXImporter::CreateBone(aiNode* node)
{
	BONE bone;
	
	m_Bone[node->mName.C_Str()] = bone;

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		CreateBone(node->mChildren[i]);
	}
}