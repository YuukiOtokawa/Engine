#include "CollisionManager.h"

#include "Component_Transform.h"
#include "Object.h"

CollisionManager* CollisionManager::m_pInstance = nullptr;
std::list<Object*> CollisionManager::m_CollisionObjects;
std::unordered_map<ObjectPair, CollisionState, ObjectPairHash> CollisionManager::m_CollisionPairs;
std::unordered_map<ObjectPair, CollisionState, ObjectPairHash> CollisionManager::m_PreviousCollisionPairs;



void CollisionManager::AddCollisionObject(Object* object)
{
    m_CollisionObjects.push_back(object);
}

void CollisionManager::RemoveCollisionObject(Object* object)
{
    m_CollisionObjects.remove(object);
    
    // このオブジェクトに関連する衝突状態を削除
    auto it = m_CollisionPairs.begin();
    while (it != m_CollisionPairs.end())
    {
        if (it->first.first == object || it->first.second == object)
        {
            it = m_CollisionPairs.erase(it);
        }
        else
        {
            ++it;
        }
    }
    
    // 前フレームの衝突状態からも削除
    it = m_PreviousCollisionPairs.begin();
    while (it != m_PreviousCollisionPairs.end())
    {
        if (it->first.first == object || it->first.second == object)
        {
            it = m_PreviousCollisionPairs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CollisionManager::CheckCollisions()
{
    // 前フレームの状態を保存
    m_PreviousCollisionPairs = m_CollisionPairs;
    
    // 現在フレームの衝突状態をリセット
    m_CollisionPairs.clear();
    
    // すべてのオブジェクトのペアをチェック
    for (auto it1 = m_CollisionObjects.begin(); it1 != m_CollisionObjects.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != m_CollisionObjects.end(); ++it2) {
            Object* obj1 = *it1;
            Object* obj2 = *it2;
            
            auto collider1 = obj1->GetComponent<Collider>();
            auto collider2 = obj2->GetComponent<Collider>();
            if (!collider1 || !collider2) continue;
            ColliderParameter::ColliderType type1 = collider1->m_ColliderType;
            ColliderParameter::ColliderType type2 = collider2->m_ColliderType;
            // 衝突判定関数を呼び出し
            if (CollisionMatrix[(int)type1][(int)type2](obj1, obj2))
            {
                // 衝突している場合、現在の状態を記録
                ObjectPair pair = NormalizeObjectPair(obj1, obj2);
                m_CollisionPairs[pair] = CollisionState::Stay; // 一旦Stayとして記録
			}

        }
    }
    
    // 衝突状態を更新（Enter, Stay, Exitの判定）
    UpdateCollisionStates();
}

CollisionState CollisionManager::GetCollisionState(Object* obj1, Object* obj2)
{
    ObjectPair pair = NormalizeObjectPair(obj1, obj2);
    auto it = m_CollisionPairs.find(pair);
    if (it != m_CollisionPairs.end())
    {
        return it->second;
    }
    return CollisionState::None;
}

void CollisionManager::SetCollisionState(Object* obj1, Object* obj2, CollisionState state)
{
    ObjectPair pair = NormalizeObjectPair(obj1, obj2);
    if (state == CollisionState::None)
    {
        m_CollisionPairs.erase(pair);
    }
    else
    {
        m_CollisionPairs[pair] = state;
    }
}

void CollisionManager::ClearCollisionStates()
{
    m_CollisionPairs.clear();
    m_PreviousCollisionPairs.clear();
}

int CollisionManager::GetCollisionPairCount() const
{
    return static_cast<int>(m_CollisionPairs.size());
}

std::list<std::pair<Object*, CollisionState>> CollisionManager::ObjectOnCollision(Object* object)
{
    std::list<std::pair<Object*, CollisionState>> collisions;
    for (const auto& [pair, state] : m_CollisionPairs)
    {
        if (pair.first == object)
        {
            collisions.emplace_back(pair.second, state);
        }
        else if (pair.second == object)
        {
            collisions.emplace_back(pair.first, state);
        }
    }
	return collisions;
}

ObjectPair CollisionManager::NormalizeObjectPair(Object* obj1, Object* obj2)
{
    if (obj1 < obj2)
    {
        return { obj1, obj2 };
    }
    else
    {
        return { obj2, obj1 };
    }
}

void CollisionManager::UpdateCollisionStates()
{
    // 現在衝突しているペアをチェック
    for (auto& [pair, currentState] : m_CollisionPairs)
    {
        auto prevIt = m_PreviousCollisionPairs.find(pair);
        
        if (prevIt != m_PreviousCollisionPairs.end())
        {
            // 前フレームでも衝突していた → Stay
            currentState = CollisionState::Stay;
        }
        else
        {
            // 前フレームでは衝突していなかった → Enter
            currentState = CollisionState::Enter;
        }
    }
    
    // 前フレームで衝突していたが今フレームで衝突していないペアをチェック → Exit
    for (const auto& [prevPair, prevState] : m_PreviousCollisionPairs)
    {
        auto currentIt = m_CollisionPairs.find(prevPair);
        if (currentIt == m_CollisionPairs.end())
        {
            // 今フレームでは衝突していない → Exit状態として追加
            m_CollisionPairs[prevPair] = CollisionState::Exit;
        }
    }

    std::unordered_map< ObjectPair, CollisionState, ObjectPairHash> toDelete;

	// 前フレームでExit状態で今フレーム衝突していないペアを削除
    for (const auto& [prevPair, prevState] : m_PreviousCollisionPairs)
    {
        if (prevState != CollisionState::Exit)
            continue;

        auto currentIt = m_CollisionPairs.find(prevPair);
        if (currentIt == m_CollisionPairs.end())
        {
            toDelete.insert({ prevPair,prevState });
        }
    }

    for (const auto& deletePair : toDelete) {
		deletePair.first.first->GetComponent<Collider>()->OnCollisionExit(deletePair.first.second);
		deletePair.first.second->GetComponent<Collider>()->OnCollisionExit(deletePair.first.first);
		m_PreviousCollisionPairs.erase(deletePair.first);
    }
}

bool CollisionManager::CollideSphereSphere(Object* obj1, Object* obj2)
{
    return false;
}

bool CollisionManager::CollideBoxBox(Object* obj1, Object* obj2)
{

    auto transform1 = obj1->GetComponent<Transform>();
    auto transform2 = obj2->GetComponent<Transform>();
    if (!transform1 || !transform2) return false;

    auto collider1 = obj1->GetComponent<Collider>();
    auto collider2 = obj2->GetComponent<Collider>();
    if (!collider1 || !collider2) return false;

    // 簡単なAABBの例として、位置と固定サイズを使用
    Vector4O pos1 = transform1->GetPosition();
    Vector4O pos2 = transform2->GetPosition();

    // 衝突判定（簡単な距離ベース AABB）
    //float collisionDistanceX = collider1->GetCollisionSize().x + collider2->GetCollisionSize().x;
    //float collisionDistanceY = collider1->GetCollisionSize().y + collider2->GetCollisionSize().y;
    //float collisionDistanceZ = collider1->GetCollisionSize().z + collider2->GetCollisionSize().z;
    bool isColliding = false;

    // AABB
    //if (abs(distance.x) <= collisionDistanceX &&
    //    abs(distance.y) <= collisionDistanceY &&
    //    abs(distance.z) <= collisionDistanceZ) {
    //    isColliding = true;
    //}

	Vector3O direction = (transform2->GetPosition() - transform1->GetPosition()).XYZ();

	Vector3O axisX = transform1->GetRight();
	float dotX = abs(direction.Dot(axisX, direction));

	Vector3O axisY = transform1->GetUp();
	float dotY = abs(direction.Dot(axisY, direction));

	Vector3O axisZ = transform1->GetForward();
	float dotZ = abs(direction.Dot(axisZ, direction));

	Vector3O scale1 = transform1->GetScale().XYZ();
	Vector3O scale2 = transform2->GetScale().XYZ();

    if (dotX <= scale1.x &&
        dotY <= scale1.y &&
		dotZ <= scale1.z) {
        isColliding = true;
    }
    else if (dotX <= scale2.x &&
             dotY <= scale2.y &&
             dotZ <= scale2.z) {
		isColliding = true;
    }

    if (isColliding)
    {
        // 衝突している場合、現在の状態を記録
        ObjectPair pair = NormalizeObjectPair(obj1, obj2);
        m_CollisionPairs[pair] = CollisionState::Stay; // 一旦Stayとして記録
    }
}

bool CollisionManager::CollideBoxSphere(Object* obj1, Object* obj2)
{
	return false;
}

bool CollisionManager::CollideMeshFieldSphere(Object* obj1, Object* obj2)
{
    return false;
}

#include "MeshFieldCollider.h"
#include "Rigidbody.h"
bool CollisionManager::CollideMeshFieldBox(Object* obj1, Object* obj2)
{
	auto transformMesh = obj1->GetComponent<Transform>();
	auto transformBox = obj2->GetComponent<Transform>();
	if (!transformBox || !transformMesh) return false;

	auto colliderMesh = obj1->GetComponent<MeshFieldCollider>();
	auto colliderBox = obj2->GetComponent<Collider>();
	if (!colliderBox || !colliderMesh) return false;

	// 簡単なAABBの例として、位置と固定サイズを使用
	Vector4O posBox = transformBox->GetPosition();
    float posBoxUnder = posBox.y - transformBox->GetScale().y;
	Vector4O posMesh = transformMesh->GetPosition();
	float height = colliderMesh->GetHeight(posBox.XYZ());

    if (height == D3D11_FLOAT32_MAX)
        return false;

    if (posBoxUnder > height + posMesh.y)
    {
		return false;
	}

    auto bodyBox = obj2->GetComponent<Rigidbody>();

    if (bodyBox) {
	    transformBox->SetPosition(Vector4O(posBox.x, posBox.y + (height - posBoxUnder), posBox.z));
        bodyBox->SetVelocity(0.0f);
    }

	return true;
}
