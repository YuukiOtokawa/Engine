#include "MeshFieldCollider.h"

#include "Component_Transform.h"
#include "Meshfield.h"

REGISTER_COMPONENT(MeshFieldCollider)

float MeshFieldCollider::GetHeight(Vector3O position)
{
	auto transform = owner->GetComponent<Transform>();
	if (!transform) return D3D11_FLOAT32_MAX;
	auto meshField = owner->GetComponent<MeshField>();
	if (!meshField) return D3D11_FLOAT32_MAX;
	Vector3O localPos = position - transform->GetPosition().XYZ();
	Vector3O scale = transform->GetScale().XYZ();
	//localPos.x /= scale.x;
	//localPos.y /= scale.y;
	//localPos.z /= scale.z;
	float halfSizeX = meshField->m_CellSize.x * meshField->m_MeshSize[0] * transform->GetScale().x * 0.5f;
	float halfSizeZ = meshField->m_CellSize.y * meshField->m_MeshSize[1] * transform->GetScale().z * 0.5f;
	if (localPos.x < -halfSizeX || localPos.x > halfSizeX ||
		localPos.z < -halfSizeZ || localPos.z > halfSizeZ) {
		return D3D11_FLOAT32_MAX; // 範囲外
	}
	// positionをメッシュフィールドのローカル座標に変換
	localPos.x += halfSizeX;
	localPos.z += halfSizeZ;
	// グリッド座標を計算
	float gridX = localPos.x / (meshField->m_CellSize.x * transform->GetScale().x);
	float gridZ = localPos.z / (meshField->m_CellSize.y * transform->GetScale().z);
	int x0 = static_cast<int>(floorf(gridX));
	int z0 = static_cast<int>(floorf(gridZ));
	int x1 = x0 + 1;
	int z1 = z0 + 1;
	if (x0 < 0 || x1 >= meshField->m_MeshSize[0] ||
		z0 < 0 || z1 >= meshField->m_MeshSize[1]) {
		return D3D11_FLOAT32_MAX; // 範囲外
	}
	//線形補間
	float sx = gridX - x0;
	float sz = gridZ - z0;
	float h00 = meshField->GetHeight(x0, z0);
	float h10 = meshField->GetHeight(x1, z0);
	float h01 = meshField->GetHeight(x0, z1);
	float h11 = meshField->GetHeight(x1, z1);
	float height = (1 - sx) * (1 - sz) * h00 + sx * (1 - sz) * h10 + (1 - sx) * sz * h01 + sx * sz * h11;
	return height;
}
