#include "ComputeParticle.h"

#include "Component_Transform.h"
#include "RenderCore.h"

REGISTER_COMPONENT(ComputeParticle)

void ComputeParticle::CreateMesh()
{
	// 頂点データとインデックスデータの生成
	std::vector<VERTEX> vertex(4);

	{
		vertex[0].position = Vector3O(
			0.0f,
			0.0f,
			0.0f
		);
		vertex[0].normal = Vector3O::Up();
		vertex[0].texcoord = Vector2O(
			0.0f, 0.0f
		);
		vertex[0].color = Vector4O::One();
	}
	{
		vertex[1].position = Vector3O(
			1.0f,
			0.0f,
			0.0f
		);
		vertex[1].normal = Vector3O::Up();
		vertex[1].texcoord = Vector2O(
			1.0f, 0.0f
		);
		vertex[1].color = Vector4O::One();
	}
	{
		vertex[2].position = Vector3O(
			0.0f,
			1.0f,
			0.0f
		);
		vertex[2].normal = Vector3O::Up();
		vertex[2].texcoord = Vector2O(
			0.0f, 1.0f
		);
		vertex[2].color = Vector4O::One();
	}
	{
		vertex[3].position = Vector3O(
			1.0f,
			1.0f,
			0.0f
		);
		vertex[3].normal = Vector3O::Up();
		vertex[3].texcoord = Vector2O(
			1.0f, 1.0f
		);
		vertex[3].color = Vector4O::One();
	}


	std::vector<UINT> index(4);
	m_NumIndices = 4;
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 3;


	m_VertexIndex.SetVertexInfo(vertex);
	m_VertexIndex.SetIndexInfo(index);
	m_VertexIndex.SetName("ParticleMesh");
	m_VertexIndex.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void ComputeParticle::CreateBuffer() {
	// 既存のバッファがあれば解放
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
	}
	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
	}

	auto vertices = m_VertexIndex.GetVertexInfo();
	// 頂点バッファ生成
	if (vertices.empty()) return;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	HRESULT hr = RenderCore::GetInstance()->GetDevice()->CreateBuffer(&bd, NULL, &m_pVertexBuffer);
	
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
		auto srv = m_pBuffer->GetSRV();
		if (srv)
			context->CSSetShaderResources(0,1,&srv);
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

    // 3. 頂点・インデックスバッファを設定する (MeshFilter::Draw()から移動)
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	RenderCore::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	RenderCore::GetInstance()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 4. プリミティブトポロジーを設定する (MeshFilter::Draw()から移動)
    RenderCore::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // 5. シェーダーとマテリアルのプロパティを設定する (すでにあなたのコードに記述されています)
	auto ownerScale = transform->GetScale();
	m_pMaterial->GetMaterial()->Aspect = Vector2O(ownerScale.x,ownerScale.y);
	m_pMaterial->SetShader();
	m_pMaterial->DrawMaterial();


	RenderCore::GetInstance()->GetDeviceContext()->DrawIndexed(m_NumIndices, 0, 0);

	

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
