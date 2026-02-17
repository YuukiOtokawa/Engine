#include "ComputeParticle.h"

#include "Component_Transform.h"
#include "RenderCore.h"

REGISTER_COMPONENT(ComputeParticle)

void ComputeParticle::CreateMesh()
{
	// パーティクルシステムでは各パーティクルを1点として描画するため、
	// メッシュデータは不要（動的頂点バッファのみ使用）
	m_NumIndices = m_NumParticles;
}

void ComputeParticle::CreateBuffer() {
	// 既存のバッファがあれば解放
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	// パーティクル数分の動的頂点バッファを作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * m_NumParticles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// NULLで初期化（データはUpdate()で毎フレーム書き込む）
	HRESULT hr = RenderCore::GetInstance()->GetDevice()->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);

	// パーティクルデータを初期化
	for (int i=0;i<m_NumParticles;i++) {
		m_Particles[i].Position = XMFLOAT4(0.0f,0.0f,0.0f,0.0f);
		m_Particles[i].Acceleration = XMFLOAT4(0.0f,0.001f,0.0f,0.0f);
		m_Particles[i].Parameter = XMFLOAT4(0.0f,0.0f,0.0f,0.0f);
		m_Particles[i].Velocity = XMFLOAT4(0.0f,0.0f,0.0f,0.0f);
		CreateParticle(i);
	}
}

ComputeParticle::ComputeParticle() : Renderer(RenderQueue::Geometry) {
	m_ClassID = CID_Component_MeshRenderer;
	// 継承関係のため、ここでマテリアルを直接newする。
	// もしくは、基底クラスのコンストラクタで初期化することも可能。
	m_pMaterial = new Material();
	
	strcpy_s(m_comShaderNameBuffer, m_pComputeShader.c_str());

	
	srand((UINT)timeGetTime());
    
	m_pBuffer = new ComputeBuffer();
	
	m_pBuffer->CreateSRVStructureBuffer(sizeof(PARTICLE), m_NumParticles);
	m_pBuffer->CreateUAVStructureBuffer(sizeof(PARTICLE), m_NumParticles);

	CreateMesh();
	CreateBuffer();
}

ComputeParticle::~ComputeParticle() {
	
}

void ComputeParticle::Update() {
	auto context = RenderCore::GetInstance()->GetDeviceContext();
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(m_pBuffer->GetSRVStructureBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		PARTICLE* pdata = (PARTICLE*)msr.pData;
		memcpy(msr.pData,&m_Particles[0],sizeof(PARTICLE)*m_NumParticles);
		context->Unmap(m_pBuffer->GetSRVStructureBuffer(), 0);
	}
	{
		if (m_pBuffer)
			m_pBuffer->SetBuffer();
		if (!m_pComputeShader.empty())
			RenderCore::GetInstance()->GetComputeShader(m_pComputeShader)->Dispatch(256,1,1);
	}
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(m_pBuffer->GetUAVStructureBuffer(),0,D3D11_MAP_READ,0,&msr);
		PARTICLE* pBufType = (PARTICLE*)msr.pData;
		memcpy(&m_Particles[0], pBufType, sizeof(PARTICLE) * m_NumParticles);
		context->Unmap(m_pBuffer->GetUAVStructureBuffer(), 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		VERTEX* v = (VERTEX*)msr.pData;
		for (int i=0;i<m_NumParticles; i++) {
			if (m_Particles[i].Parameter.x <= 0.0f) {
				CreateParticle(i);
			}
			v[i].position = m_Particles[i].Position.XYZ();
			v[i].color = XMFLOAT4(1,1,1,1);
		}
		context->Unmap(m_pVertexBuffer,0);
	}
}

#include "Component_MeshFilter.h"
void ComputeParticle::Render() {
	if (!m_bIsVisible) return;




	// 2. Transformコンポーネントを取得
	auto transform = owner->GetComponent<Transform>();
	if (!transform) return;
	XMMATRIX scale, angle, translation;
	{
		if (owner->IsChild() && owner->GetParent()->GetComponent<Transform>()) {
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
			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto quat = transform->GetQuaternion().ToFloat4();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
			angle = XMMatrixRotationQuaternion(XMLoadFloat4(quat));
			translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
		}

	}

	transform->SetTransformMatrix(translation);

    // 行列をレンダラーに設定する
    RenderCore::GetInstance()->SetTranslationMatrix(translation);
    RenderCore::GetInstance()->SetScaleMatrix(scale);
    RenderCore::GetInstance()->SetAngleMatrix(angle);

    // 3. 頂点バッファを設定する
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	RenderCore::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // 4. プリミティブトポロジーを設定する
    RenderCore::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // 5. シェーダーとマテリアルのプロパティを設定する
	auto ownerScale = transform->GetScale();
	m_pMaterial->GetMaterial()->Aspect = Vector2O(ownerScale.x,ownerScale.y);
	m_pMaterial->SetShader();
	m_pMaterial->DrawMaterial();

	// パーティクル数分描画（インデックスバッファ不要）
	RenderCore::GetInstance()->GetDeviceContext()->Draw(m_NumParticles, 0);

	

}

void ComputeParticle::DrawGUI() {
	ImGui::Indent();
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

	if (ImGui::InputText("Compute", m_comShaderNameBuffer, IM_ARRAYSIZE(m_comShaderNameBuffer), flags)) {
		
		m_pComputeShader = m_comShaderNameBuffer;
	}

	m_pMaterial->DrawGUI();
	ImGui::Unindent();
}

void ComputeParticle::ImportFile(YAML::Node& node)
{
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["materialFileID"]) {
		m_pMaterial = Editor::GetInstance()->GetMaterialByFileID(node["materialFileID"].as<int>());
	}
}

void ComputeParticle::ExportComponent(YAML::Emitter& out)
{
	out << YAML::Key << "materialFileID" << YAML::Value << m_pMaterial->GetFileID();
}

void ComputeParticle::AddExportList()
{
	SceneExporter::AddExportList(this);
	SceneExporter::AddExportList(m_pMaterial);
}

void ComputeParticle::InitializeTag() {
	//owner->SetTag(GameObjectTagLayer::ObjectTag);
}

void ComputeParticle::CreateParticle(int i) {
	PARTICLE p;
	p.Position = Vector4O(0.0f,0.0f,0.0f,0.0f);
	p.Acceleration = Vector4O(0.0f,-0.002f,0.0f,0.0f);
    
	float v = ((float)rand() / (float)RAND_MAX) * 90.0f + 45.0f;
	p.Velocity = Vector4O(
		cosf(XMConvertToRadians(v)) * 0.087f,
		sinf(XMConvertToRadians(v)) * 0.098f,
		0.0f,
		0.0f);
	p.Parameter.x = (float)(rand() & 500) + 50.0f;
    
	m_Particles[i] = p;
}
