#include "PrefabSpawner.h"
#include "Object.h"
#include "Component_Transform.h"
#include "Keyboard.h"
#include "EngineConsole.h"

#include "ScriptFactory.h"

REGISTER_SCRIPT(PrefabSpawner)
REGISTERCLASS(PrefabSpawner);

void PrefabSpawner::Start()
{
    EngineConsole::Log("PrefabSpawner: Start - Prefabが設定されています: %s",
                       m_prefabToSpawn.IsValid() ? "Yes" : "No");
}

void PrefabSpawner::Update()
{
    // Spaceキーを押したらPrefabをスポーン
    if (Keyboard::GetInstance()->GetKeyDown(KK_SPACE)) {
        SpawnPrefab();
    }

    // Cキーを押したら生成したオブジェクトをクリア
    if (Keyboard::GetInstance()->GetKeyDown(KK_C)) {
        ClearSpawned();
    }
}

void PrefabSpawner::SpawnPrefab()
{
    // Prefabが設定されているか確認
    if (!m_prefabToSpawn.IsValid()) {
        EngineConsole::LogWarning("PrefabSpawner: Prefabが設定されていません");
        return;
    }

    // 指定された数だけインスタンス化
    for (int i = 0; i < m_spawnCount; i++) {
        Object* spawned = m_prefabToSpawn.Instantiate();
        if (spawned) {
            // スポーン位置を設定
            Transform* transform = spawned->GetComponent<Transform>();
            if (transform) {
                Vector3O currentPos = transform->GetPosition().XYZ();
                Vector3O newPos = currentPos + m_spawnOffset * (float)i;
                transform->SetPosition(newPos);
            }

            m_spawnedObjects.push_back(spawned);
            EngineConsole::Log("PrefabSpawner: オブジェクトを生成しました (%d/%d)", i + 1, m_spawnCount);
        }
    }
}

void PrefabSpawner::ClearSpawned()
{
    for (Object* obj : m_spawnedObjects) {
        if (obj) {
            delete obj;
        }
    }
    m_spawnedObjects.clear();
    EngineConsole::Log("PrefabSpawner: 生成されたオブジェクトをすべて削除しました");
}
