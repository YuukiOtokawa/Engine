#include "FieldMeshRenderer.h"

#include "MainEngine.h"

void FieldMeshRenderer::UpdateComponent()
{
	if (m_NumMeshesX == 0 || m_NumMeshesZ == 0) return;

	if (m_NumMeshesX != m_LastNumMeshesX || m_NumMeshesZ != m_LastNumMeshesZ)
	{

	}
}

void FieldMeshRenderer::Draw()
{
	// プリミティブトポロジ設定
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアル設定
	if (m_pMaterial) {
		m_pMaterial->DrawMaterial();
	}
	for (int z = 0; z < m_NumMeshesZ; z++)
	{
		for (int x = 0; x < m_NumMeshesX; x++)
		{
			int meshIndex = z * m_NumMeshesX + x;
			// 頂点バッファ設定
			UINT stride = sizeof(VERTEX);
			UINT offset = 0;
			MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer[meshIndex], &stride, &offset);
			// インデックスバッファ設定
			MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer[meshIndex], DXGI_FORMAT_R32_UINT, 0);
			// ポリゴン描画
			MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(6 * 6, 0, 0);
		}
	}
}
