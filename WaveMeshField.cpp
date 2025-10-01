#include "WaveMeshField.h"

REGISTER_COMPONENT(WaveMeshField)

void WaveMeshField::WaveLinear()
{
	auto vertices = m_VertexIndex.GetVertexInfo();
	for (auto& v : vertices) {
		float wave = sinf((v.position.x + v.position.z) / m_WaveLength + m_Time) * m_WaveHeight;
		v.position.y = wave;
	}
	m_VertexIndex.SetVertexInfo(vertices);
}

void WaveMeshField::WaveCircular()
{
	auto vertices = m_VertexIndex.GetVertexInfo();
	for (auto& v : vertices) {
		// 2D‹——£‚ðŒvŽZ
		float dist = sqrtf(v.position.x * v.position.x + v.position.z * v.position.z);
		// ”g‚ÌŒvŽZ
		float wave = sinf(dist / m_WaveLength - m_Time) * m_WaveHeight;
		v.position.y = wave;
	}
	m_VertexIndex.SetVertexInfo(vertices);
}

WaveMeshField::WaveMeshField()
{
}

WaveMeshField::~WaveMeshField()
{
}

void WaveMeshField::Update()
{
	m_Time += 0.016f * m_WaveSpeed;
	switch (m_WaveType)
	{
	case WaveMeshField::WaveType::Linear:
		WaveLinear();
		break;
	case WaveMeshField::WaveType::Circular:
		WaveCircular();
		break;
	default:
		break;
	}
	CreateBuffer();
}

void WaveMeshField::DrawGUI()
{
	ImGui::Indent();

	ImGui::InputInt2("Vertex", &m_CurrentVertex[0]);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		if (m_CurrentVertex[0] < 0) m_CurrentVertex[0] = 0;
		if (m_CurrentVertex[0] >= m_MeshSize[0]) m_CurrentVertex[0] = (int)m_MeshSize[0] - 1;
		if (m_CurrentVertex[1] < 0) m_CurrentVertex[1] = 0;
		if (m_CurrentVertex[1] >= m_MeshSize[1]) m_CurrentVertex[1] = (int)m_MeshSize[1] - 1;
		m_CurrentFocusVertexInfo = m_VertexIndex.GetVertexInfo()[m_CurrentVertex[0] * (int)m_MeshSize[1] + m_CurrentVertex[1]];
	}

	ImGui::InputFloat3("Position", (float*)&m_CurrentFocusVertexInfo.position, "%.1f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		m_VertexIndex.SetVertexInfo(&m_CurrentFocusVertexInfo, (int)(m_CurrentVertex[0] * (int)m_MeshSize[1] + m_CurrentVertex[1]));
		CreateBuffer();
	}

	ImGui::InputInt2("MeshSize", &m_MeshSize[0]);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		CreateMesh();
		CreateBuffer();
	}

	ImGui::DragFloat("WaveHeight", &m_WaveHeight, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("WaveLength", &m_WaveLength, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("WaveSpeed", &m_WaveSpeed, 0.1f, 0.0f, 100.0f);

	const char* waveTypes[] = { "Linear", "Circular" };
	int currentWaveType = static_cast<int>(m_WaveType);
	if (ImGui::Combo("WaveType", &currentWaveType, waveTypes, IM_ARRAYSIZE(waveTypes))) {
		m_WaveType = static_cast<WaveType>(currentWaveType);
	}

	m_pMaterial->DrawGUI();
	ImGui::Unindent();
}

void WaveMeshField::InitializeTag()
{
}
