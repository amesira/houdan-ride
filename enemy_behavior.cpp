//===================================================
// enemy_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//===================================================
#include "enemy_behavior.h"

#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "mi_math.h"

#include "transform_component.h"
#include "rigidbody_component.h"
#include "collider_component.h"
#include "image_component.h"

EnemyBehavior::EnemyBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<EnemyBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();
    m_image = owner->GetComponent<ImageComponent>();
}

EnemyBehavior::~EnemyBehavior()
{

}

void EnemyBehavior::Update(IScene* pScene)
{
    // プレイヤー取得
    if (!m_playerTransform) {
        GameObject* playerObj = pScene->GetGameObjectByName("Player");
        if (playerObj) {
            m_playerTransform = playerObj->GetComponent<TransformComponent>();
        }
        return;
    }

    // プレイヤー方向
    XMFLOAT3 dirToPlayer = {
        m_playerTransform->GetPosition().x - m_transform->GetPosition().x,
        0.0f,
        m_playerTransform->GetPosition().z - m_transform->GetPosition().z
    };
    dirToPlayer = MiMath::Normalize(dirToPlayer);

    XMFLOAT3 velocity = {
        dirToPlayer.x * 2.0f,
        m_rigidbody->GetVelocity().y,
        dirToPlayer.z * 2.0f
    };
    m_rigidbody->SetVelocity(velocity);

    // プレイヤーに触れた場合
    for(int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto collisionData = m_collider->GetCollisionData(i);

        if(collisionData.GetCollisionEnter()) {
            GameObject* otherObj = collisionData.m_other->GetOwner();
            if(otherObj->GetName() == "Player") {
                // ダメージ処理など
                GetOwner()->Destroy();
            }
        }
    }
}
