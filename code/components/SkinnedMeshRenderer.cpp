#include "SkinnedMeshRenderer.h"

#include "Component_Transform.h"
#include "MainEngine.h"

#include "RenderCore.h"

REGISTER_COMPONENT(SkinnedMeshRenderer)

XMMATRIX ConvertMatrix(const aiMatrix4x4& from) {
	XMMATRIX to;
	to.r[0] = XMVectorSet(from.a1, from.b1, from.c1, from.d1);
	to.r[1] = XMVectorSet(from.a2, from.b2, from.c2, from.d2);
	to.r[2] = XMVectorSet(from.a3, from.b3, from.c3, from.d3);
	to.r[3] = XMVectorSet(from.a4, from.b4, from.c4, from.d4);
	return XMMatrixTranspose(to);
}

SkinnedMeshRenderer::SkinnedMeshRenderer()
{
	m_ClassID = CID_Component_SkinnedMeshRenderer;

	m_pMaterial = new Material();
	m_pMaterial->SetVertexShaderKey("unlit");
	m_pMaterial->SetPixelShaderKey("unlit");

	MATERIAL material;
	m_pMaterial->SetMaterial(material);

	m_pAnimation = new std::unordered_map<std::string, const aiScene*>;
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	if (m_pAiScene)
		for (unsigned int m = 0; m < m_pAiScene->mNumMeshes; m++)
		{
			m_pVertexBuffer[m]->Release();
			m_pIndexBuffer[m]->Release();
		}

	delete[] m_pVertexBuffer;
	delete[] m_pIndexBuffer;

	delete[] m_pDeformVertex;

	aiReleaseImport(m_pAiScene);

	if (m_pAnimation)
	for (auto pair = m_pAnimation->begin(); pair != m_pAnimation->end(); ++pair)
	{
		aiReleaseImport(pair->second);
	}

}

void SkinnedMeshRenderer::SetMeshData(std::vector<DEFORM_VERTEX>* deformVertex, std::unordered_map<std::string, BONE>* boneMap, const aiScene* aiScene)
{
	m_pBoneMap = boneMap;
	m_pAiScene = aiScene;
	InitializeBuffers();
}


void SkinnedMeshRenderer::SetAnimationData(std::unordered_map<std::string, const aiScene*>* aiAnimation)
{
	m_pAnimation = aiAnimation;
}

void SkinnedMeshRenderer::InitializeBuffers() {
	if (!m_pAiScene || m_pAiScene->mNumMeshes == 0) return;

	m_pVertexBuffer = new ID3D11Buffer * [m_pAiScene->mNumMeshes];
	m_pIndexBuffer = new ID3D11Buffer * [m_pAiScene->mNumMeshes];

	m_pDeformVertex = new std::vector<DEFORM_VERTEX>[m_pAiScene->mNumMeshes];

	m_pTextureFID = new std::unordered_map<std::string, int>();

	for (unsigned int m = 0; m < m_pAiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_pAiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX* vertex = new VERTEX[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].position = Vector4O(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].normal = Vector4O(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].texcoord = Vector2O(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
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

			HRESULT hr = MainEngine::GetInstance()->GetRenderCore()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

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

			MainEngine::GetInstance()->GetRenderCore()->GetDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer[m]);

			delete[] index;
		}



		//変形後頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX deformVertex;
			deformVertex.position = mesh->mVertices[v];
			deformVertex.normal = mesh->mNormals[v];
			deformVertex.boneNum = 0;

			for (unsigned int b = 0; b < 4; b++)
			{
				deformVertex.boneName[b] = "";
				deformVertex.boneWeight[b] = 0.0f;
			}

			m_pDeformVertex[m].push_back(deformVertex);
		}


		//ボーンデータ初期化
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];

			m_pBoneMap->at(bone->mName.C_Str()).offsetMatrix = bone->mOffsetMatrix;

			//変形後頂点にボーンデータ格納
			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				aiVertexWeight weight = bone->mWeights[w];

				//if (m_pDeformVertex[m][weight.mVertexId].boneNum >= 4) continue;
				int num = m_pDeformVertex[m][weight.mVertexId].boneNum;

				m_pDeformVertex[m][weight.mVertexId].boneWeight[num] = weight.mWeight;
				m_pDeformVertex[m][weight.mVertexId].boneName[num] = bone->mName.C_Str();
				m_pDeformVertex[m][weight.mVertexId].boneNum++;

				assert(m_pDeformVertex[m][weight.mVertexId].boneNum <= 4);
			}
		}

		


	}

	auto device = MainEngine::GetInstance()->GetRenderCore()->GetDevice();

	for (int i = 0; i < m_pAiScene->mNumTextures; i++) {
		aiTexture* aitexture = m_pAiScene->mTextures[i];

		ID3D11ShaderResourceView* texture;

		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(
			reinterpret_cast<const uint8_t*>(aitexture->pcData),
			static_cast<size_t>(aitexture->mWidth),
			WIC_FLAGS_NONE,
		 &metadata,
		 image
		);
		CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		Texture* tex = new Texture;
		tex->shaderResourceView = texture;
		tex->width = metadata.width;
		tex->height = metadata.height;
		tex->toExport = false;

		auto texFID = MainEngine::GetInstance()->GetRenderCore()->AddTexture(tex);

		m_pTextureFID->emplace(aitexture->mFilename.data, texFID);
	}

	Material material;

	MATERIAL mBuffer;

	material.SetMaterial(mBuffer);


}

void SkinnedMeshRenderer::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 parentMatrix) {
	BONE* bone = &(*m_pBoneMap)[node->mName.C_Str()];

	aiMatrix4x4 worldMatrix;
	// 上下で掛け算が逆になる
	worldMatrix = parentMatrix * bone->animationMatrix;

	bone->matrix = worldMatrix * bone->offsetMatrix;

	for (unsigned int n = 0; n < node->mNumChildren; n++) {
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}

void SkinnedMeshRenderer::GetTexture()
{
	if (!m_pAiScene) return;
	RenderCore* renderer = MainEngine::GetInstance()->GetRenderCore();
	for (UINT i = 0; i < m_pAiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_pAiScene->mTextures[i];
		ID3D11ShaderResourceView* texture;
		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(
			reinterpret_cast<const uint8_t*>(aitexture->pcData),
			static_cast<size_t>(aitexture->mWidth),
			WIC_FLAGS_NONE,
		 &metadata,
		 image
		);
		CreateShaderResourceView(renderer->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);
		//m_pTextureFID->at(aitexture->mFilename.data) = texture;
	}
}

void SkinnedMeshRenderer::Update() {
	if (!m_pAiScene || !m_pDeformVertex || !m_pBoneMap || m_pAiScene->mNumMeshes == 0) return;

	aiAnimation* currentAnimation = nullptr;
	aiAnimation* nextAnimation = nullptr;

	if (m_CurrentAnimation != "") {
		currentAnimation = m_pAnimation->at(m_CurrentAnimation)->mAnimations[0];

		if (m_NextAnimation != "") {
			nextAnimation = m_pAnimation->at(m_NextAnimation)->mAnimations[0];

		}
	}
	else {
		if (m_NextAnimation != "") {
			m_CurrentAnimation = m_NextAnimation;
			currentAnimation = m_pAnimation->at(m_CurrentAnimation)->mAnimations[0];
			nextAnimation = m_pAnimation->at(m_NextAnimation)->mAnimations[0];
		}
		else
			return;
	}
	


	for (auto pair : *m_pBoneMap) {

		BONE* bone = &(m_pBoneMap->at(pair.first));

		aiNodeAnim* currentNodeAnim = nullptr;
		aiNodeAnim* nextNodeAnim = nullptr;

		// CurrentAnimation
		for (unsigned int c = 0; c < currentAnimation->mNumChannels; c++) {
			if (currentAnimation->mChannels[c]->mNodeName == aiString(pair.first)) {
				currentNodeAnim = currentAnimation->mChannels[c];
				break;
			}
		}
		
		aiQuaternion currentRotation;
		aiVector3D currentPosition;
		int currentFrameKey;

		if (currentNodeAnim) {
			currentFrameKey = m_CurrentAnimationFrame % currentNodeAnim->mNumPositionKeys;
			currentRotation = currentNodeAnim->mRotationKeys[currentFrameKey].mValue;

			currentFrameKey = m_CurrentAnimationFrame % currentNodeAnim->mNumPositionKeys;
			currentPosition = currentNodeAnim->mPositionKeys[currentFrameKey].mValue;
		}


		if (m_CurrentAnimation == m_NextAnimation || !nextAnimation) {
			bone->animationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), currentRotation, currentPosition);
			continue;
		}
		else
		{
			// NextAnimation
			for (unsigned int c = 0; c < nextAnimation->mNumChannels; c++) {
				if (nextAnimation->mChannels[c]->mNodeName == aiString(pair.first)) {
					nextNodeAnim = nextAnimation->mChannels[c];
					break;
				}
			}

			aiQuaternion nextRotation;
			aiVector3D nextPosition;
			int nextFrameKey;

			if (nextNodeAnim) {
				nextFrameKey = m_NextAnimationFrame % nextNodeAnim->mNumPositionKeys;
				nextRotation = nextNodeAnim->mRotationKeys[nextFrameKey].mValue;

				nextFrameKey = m_NextAnimationFrame % nextNodeAnim->mNumPositionKeys;
				nextPosition = nextNodeAnim->mPositionKeys[nextFrameKey].mValue;
			}

			// 線形補間
			aiVector3D pos;
			pos = currentPosition * (1.0f - m_BlendRate) + nextPosition * m_BlendRate;

			aiQuaternion rot;
			aiQuaternion::Interpolate(rot, currentRotation, nextRotation, m_BlendRate);

			bone->animationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
		}
	}

	//再帰的にボーンマトリクスを更新
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
										 aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));

	UpdateBoneMatrix(m_pAiScene->mRootNode, aiMatrix4x4());

	if (m_CurrentAnimation != m_NextAnimation)
	{
		m_BlendRate += 1.0f / (60.0f * m_AnimationBlendTime); // 60fpsでm_BlendTime秒かけて補間
		if (m_BlendRate >= 1.0f) {
			m_CurrentAnimation = m_NextAnimation;
			m_CurrentAnimationFrame = m_NextAnimationFrame;
			m_BlendRate = 0.0f;
		}
	}


	//頂点変換（CPUスキニング）
	for (unsigned int m = 0; m < m_pAiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_pAiScene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Map(m_pVertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX* vertex = (VERTEX*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
			DEFORM_VERTEX* deformVertex = &m_pDeformVertex[m][v];

			aiMatrix4x4 matrix[4];
			matrix[0] = (*m_pBoneMap)[deformVertex->boneName[0]].matrix;
			matrix[1] = (*m_pBoneMap)[deformVertex->boneName[1]].matrix;
			matrix[2] = (*m_pBoneMap)[deformVertex->boneName[2]].matrix;
			matrix[3] = (*m_pBoneMap)[deformVertex->boneName[3]].matrix;

			float weight;
			weight = deformVertex->boneWeight[0]
				+ deformVertex->boneWeight[1]
				+ deformVertex->boneWeight[2]
				+ deformVertex->boneWeight[3];

			aiMatrix4x4 outMatrix;
			outMatrix = matrix[0] * (deformVertex->boneWeight[0] / weight) +
				matrix[1] * (deformVertex->boneWeight[1] / weight) +
				matrix[2] * (deformVertex->boneWeight[2] / weight) +
				matrix[3] * (deformVertex->boneWeight[3] / weight);

			deformVertex->position = mesh->mVertices[v];
			deformVertex->position *= outMatrix;

			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;

			deformVertex->normal = mesh->mNormals[v];
			deformVertex->normal *= outMatrix;

			vertex[v].position = Vector4O(deformVertex->position.x, deformVertex->position.y, deformVertex->position.z);
			vertex[v].normal = Vector4O(deformVertex->normal.x, deformVertex->normal.y, deformVertex->normal.z);
			vertex[v].texcoord = Vector2O(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			vertex[v].color = Vector4O::One();
		}
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Unmap(m_pVertexBuffer[m], 0);
	}

	m_CurrentAnimationFrame++;
}

void SkinnedMeshRenderer::Render() {

	if (!m_pVertexBuffer || !m_pIndexBuffer || !m_pMaterial) return;


	// プリミティブトポロジ設定
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.textureEnable = true;

	auto transform = owner->GetComponent<Transform>();
	if (!transform) return;
	XMMATRIX scale, angle, translation;
	{
		if (owner->IsChild()) {
			/// 子オブジェクトのtransformは親オブジェクトからの相対位置であるため、親オブジェクトのTransformを考慮
			auto parentTransform = owner->GetParent()->GetComponent<Transform>();

			auto parentScale = parentTransform->GetScale();
			auto parentRotation = parentTransform->GetRotation().ToRadian();
			auto parentPosition = parentTransform->GetPosition();

			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x * parentScale.x, objectScale.y * parentScale.y, objectScale.z * parentScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x + parentRotation.x, objectRotation.y + parentRotation.y, objectRotation.z + parentRotation.z);
			translation = XMMatrixTranslation(
				objectPosition.x + parentPosition.x,
				objectPosition.y + parentPosition.y,
				objectPosition.z + parentPosition.z
			);
		}
		else {
			auto objectScale = transform->GetScale() * 0.01f;
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
			translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
		}

	}

	// 行列をレンダラーに設定する
	MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);


	for (unsigned int m = 0; m < m_pAiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_pAiScene->mMeshes[m];

		// マテリアル設定
		aiString texture;

		aiMaterial* aimaterial = m_pAiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		m_pMaterial->SetTexture(m_pTextureFID->at(texture.data));

		m_pMaterial->SetShader();
		m_pMaterial->DrawMaterial();


		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

void SkinnedMeshRenderer::RenderShadow()
{
	if (!m_pVertexBuffer || !m_pIndexBuffer || !m_pMaterial) return;


	// プリミティブトポロジ設定
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.textureEnable = true;

	auto transform = owner->GetComponent<Transform>();
	if (!transform) return;
	XMMATRIX scale, angle, translation;
	{
		if (owner->IsChild()) {
			/// 子オブジェクトのtransformは親オブジェクトからの相対位置であるため、親オブジェクトのTransformを考慮
			auto parentTransform = owner->GetParent()->GetComponent<Transform>();

			auto parentScale = parentTransform->GetScale();
			auto parentRotation = parentTransform->GetRotation().ToRadian();
			auto parentPosition = parentTransform->GetPosition();

			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x * parentScale.x, objectScale.y * parentScale.y, objectScale.z * parentScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x + parentRotation.x, objectRotation.y + parentRotation.y, objectRotation.z + parentRotation.z);
			translation = XMMatrixTranslation(
				objectPosition.x + parentPosition.x,
				objectPosition.y + parentPosition.y,
				objectPosition.z + parentPosition.z
			);
		}
		else {
			auto objectScale = transform->GetScale() * 0.01f;
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
			translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
		}

	}

	// 行列をレンダラーに設定する
	MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);


	for (unsigned int m = 0; m < m_pAiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_pAiScene->mMeshes[m];

		// マテリアル設定
		aiString texture;

		aiMaterial* aimaterial = m_pAiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		m_pMaterial->SetTexture(m_pTextureFID->at(texture.data));

		//m_pMaterial->SetShader();
		m_pMaterial->DrawMaterial();


		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

void SkinnedMeshRenderer::DrawGUI() {
	static int animationIndex = -1;

	animationIndex = (int)std::distance(
		m_pAnimation->begin(),
		m_pAnimation->find(m_CurrentAnimation)
	);

	ImGui::Text("FBXModel:");
	ImGui::Indent();

	if (m_pAiScene) {
		ImGui::Text("%s", m_pAiScene->mName.C_Str());
	}
	else {
		ImGui::Text("No model assigned.");
	}
	FBXImporter importer;
	if (ImGui::Button("Set Mesh", ImVec2(150.0f, 30.0f)))
	{
		m_MeshFilePath = OpenImportFileDialog();

		if (!m_MeshFilePath.empty()) {
			importer.LoadFBX(m_MeshFilePath.c_str());
			SetMeshData(nullptr, importer.GetBone(), importer.GetAiScene());
			InitializeBuffers();
		}
	}

	ImGui::Unindent();

	ImGui::Indent();

	if (m_CurrentAnimation.empty()) {
		ImGui::Text("No Animation Playing.");
	}
	else {
		ImGui::Text("Current Animation: %s", m_CurrentAnimation.c_str());
	}

	if (m_NextAnimation != m_CurrentAnimation && !m_NextAnimation.empty()) {
		ImGui::Text("Next Animation: %s", m_NextAnimation.c_str());
		ImGui::Text("Swap Frame: %d", m_NextAnimationFrame);
	}

	ImGui::InputInt("Swap Frame", &m_NextAnimationFrame);

	if (!m_pAnimation->empty()) {
		std::vector<const char*> animationNames;
		for (const auto& pair : *m_pAnimation) {
			animationNames.push_back(pair.first.c_str());
		}
		if (ImGui::Combo("Animations", &animationIndex, animationNames.data(), static_cast<int>(animationNames.size()))) {
			auto it = m_pAnimation->begin();
			std::advance(it, animationIndex);
			SetNextAnimation(it->first);
		}
	}

	std::string filepath;

	if (ImGui::Button("Add Animation")) {
		filepath = OpenImportFileDialog();
	}
	if (!filepath.empty()) {
		auto animation = importer.LoadAnimation(filepath.c_str());
		m_pAnimation->emplace(animation.second, animation.first);
		m_AnimationFilePathMap.emplace(animation.second, filepath);
	}

	m_pMaterial->DrawGUI();

	ImGui::Unindent();
}

void SkinnedMeshRenderer::SetNextAnimation(std::string animationKey)
{
	if (m_pAnimation->count(animationKey) == 0) return;
	if (m_CurrentAnimation == animationKey) return;
	if (!m_pAnimation->at(animationKey)->HasAnimations()) return;
	m_NextAnimation = animationKey;
	m_NextAnimationFrame = 0;
}

void SkinnedMeshRenderer::AddExportList()
{
	SceneExporter::AddExportList(this);
	if (m_pMaterial)
		m_pMaterial->AddExportList();
}

void SkinnedMeshRenderer::ExportComponent(YAML::Emitter& out)
{
	out << YAML::Key << "meshFilePath" << YAML::Value << m_MeshFilePath;
	out << YAML::Key << "animationCount" << YAML::Value << (int)m_pAnimation->size();

	out << YAML::Key << "animations" << YAML::Value << YAML::BeginSeq;
	for (const auto& pair : *m_pAnimation) {
		out << YAML::BeginMap;
		out << YAML::Key << "name" << YAML::Value << pair.first;
		out << YAML::Key << "filePath" << YAML::Value << m_AnimationFilePathMap[pair.first];
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::Key << "materialFileID" << YAML::Value << m_pMaterial->GetFileID();
}

void SkinnedMeshRenderer::ImportFile(YAML::Node& node)
{
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["meshFilePath"]) {
		m_MeshFilePath = node["meshFilePath"].as<std::string>();
		FBXImporter importer;
		if (!m_MeshFilePath.empty()) {
			importer.LoadFBX(m_MeshFilePath.c_str());
			SetMeshData(nullptr, importer.GetBone(), importer.GetAiScene());
			InitializeBuffers();
		}
	}
	if (node["animations"]) {
		for (const auto& anim : node["animations"]) {
			std::string name = anim["name"].as<std::string>();
			std::string filePath = anim["filePath"].as<std::string>();
			m_AnimationFilePathMap[name] = filePath;

			FBXImporter animImporter;
			animImporter.LoadFBX(filePath.c_str());
			(*m_pAnimation)[name] = animImporter.GetAiScene();
		}
	}
	if (node["materialFileID"]) {
		m_pMaterial = Editor::GetInstance()->GetMaterialByFileID(node["materialFileID"].as<int>());
	}
}

void SkinnedMeshRenderer::InitializeTag()
{


}
