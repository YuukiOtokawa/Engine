#pragma once

#include "Script.h"
#include "PrefabAsset.h"
#include "VectorO.h"

class PrefabSpawner : public Script
{
private:
    // インスペクターで設定可能なPrefab
    PrefabAsset m_prefabToSpawn;

    // スポーン位置のオフセット
    Vector3O m_spawnOffset;

    // 生成する数
    int m_spawnCount;

    // 生成されたオブジェクトのリスト
    std::vector<Object*> m_spawnedObjects;

public:
    PrefabSpawner()
        : m_spawnOffset(0.0f, 0.0f, 0.0f)
        , m_spawnCount(1)
    {
        // プロパティを登録
        AddProperty("Prefab", PropertyType::PREFAB, &m_prefabToSpawn);
        AddProperty("Spawn Offset", PropertyType::VECTOR3, &m_spawnOffset);
        AddProperty("Spawn Count", PropertyType::INT, &m_spawnCount);
    }

    void Start() override;
    void Update() override;

    /// @brief Prefabをインスタンス化する
    void SpawnPrefab();

    /// @brief 生成されたオブジェクトをすべて削除
    void ClearSpawned();

    const char* GetScriptName() const override;
};
