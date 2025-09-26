#pragma once

#include <list>
#include <tuple>
#include <map>
#include <unordered_map>

#include "Collider.h"

using namespace ColliderParameter;

// 使い方
//
// Colliderをオブジェクトにアタッチ
// Colliderを持つオブジェクトはCollisionManagerに登録され、毎フレーム衝突チェックが行われる
// 衝突状態はCollisionManagerが管理し、該当オブジェクトのコンポーネントのOnCollisionEnter, OnCollisionStay, OnCollisionExitが呼ばれる
//

enum CollisionState {
    None,
    Enter,
    Stay,
    Exit
};

class Object;

// オブジェクトのペアを表すための構造体
struct ObjectPair {
    Object* first;
    Object* second;
    
    // マップで使用するための比較演算子
    bool operator<(const ObjectPair& other) const {
        if (first != other.first) 
            return first < other.first;
        return second < other.second;
    }
    
    // ハッシュマップで使用するための等価演算子
    bool operator==(const ObjectPair& other) const {
        return (first == other.first && second == other.second) ||
               (first == other.second && second == other.first);
    }
};

// ObjectPairのハッシュ関数
struct ObjectPairHash {
    std::size_t operator()(const ObjectPair& pair) const {
        // より小さいポインタを先にしてハッシュを計算することで
        // (A,B)と(B,A)が同じハッシュ値を持つようにする
        Object* p1 = pair.first < pair.second ? pair.first : pair.second;
        Object* p2 = pair.first < pair.second ? pair.second : pair.first;
        
        std::size_t h1 = std::hash<Object*>{}(p1);
        std::size_t h2 = std::hash<Object*>{}(p2);
        return h1 ^ (h2 << 1);
    }
};

class CollisionManager
{
    static std::list<Object*> m_CollisionObjects;
    static CollisionManager* m_pInstance;
    
    // オブジェクトペアとその衝突状態を管理するマップ
    static std::unordered_map<ObjectPair, CollisionState, ObjectPairHash> m_CollisionPairs;
    
    // 前フレームの衝突状態を保持するマップ
    static std::unordered_map<ObjectPair, CollisionState, ObjectPairHash> m_PreviousCollisionPairs;

    CollisionManager() = default;
public:
    static CollisionManager* GetInstance()
    {
        if (m_pInstance == nullptr)
        {
            m_pInstance = new CollisionManager();
        }
        return m_pInstance;
    }
    ~CollisionManager() = default;
    
    void AddCollisionObject(Object* object);
    void RemoveCollisionObject(Object* object);
    void CheckCollisions();
    
    // オブジェクトペアの衝突状態を取得
    CollisionState GetCollisionState(Object* obj1, Object* obj2);
    
    // 特定のオブジェクトとの衝突状態を設定（内部使用）
    void SetCollisionState(Object* obj1, Object* obj2, CollisionState state);
    
    // 衝突状態をクリア
    void ClearCollisionStates();
    
    // デバッグ用：現在の衝突ペアの数を取得
    int GetCollisionPairCount() const;

    std::list<std::pair<Object*, CollisionState>> ObjectOnCollision(Object* object);

private:
    // オブジェクトペアを正規化（常により小さいポインタを最初に配置）
    static ObjectPair NormalizeObjectPair(Object* obj1, Object* obj2);
    
    // 衝突状態を更新（Enter, Stay, Exitの判定）
    void UpdateCollisionStates();


    // 衝突判定関数
    static bool CollideSphereSphere(Object* obj1, Object* obj2);
    static bool CollideBoxBox(Object* obj1, Object* obj2);
    static bool CollideBoxSphere(Object* obj1, Object* obj2);
    static bool CollideSphereBox(Object* obj1, Object* obj2) { return CollideBoxSphere(obj2, obj1); }
    static bool CollideMeshFieldSphere(Object* obj1, Object* obj2);
    static bool CollideSphereMeshField(Object* obj1, Object* obj2) { return CollideMeshFieldSphere(obj2, obj1); }
    static bool CollideMeshFieldBox(Object* obj1, Object* obj2);
    static bool CollideBoxMeshField(Object* obj1, Object* obj2) { return CollideMeshFieldBox(obj2, obj1); }

    using CollisionFunc = bool(*)(Object*, Object*);

    CollisionFunc CollisionMatrix[(int)ColliderType::COLLIDER_TYPE_MAX][(int)ColliderType::COLLIDER_TYPE_MAX] = {
        { CollideBoxBox,       CollideBoxSphere,        CollideBoxMeshField     },
        { CollideSphereBox,    CollideSphereSphere,     CollideSphereMeshField  },
        { CollideMeshFieldBox, CollideMeshFieldSphere,  CollideMeshFieldBox     }
    };

};
