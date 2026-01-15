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
#include "mi_fps.h"

#include "transform_component.h"
#include "rigidbody_component.h"
#include "collider_component.h"
#include "image_component.h"

#include "particle_manager.h"
#include "player_behavior.h"
#include "ball_behavior.h"

EnemyBehavior::EnemyBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<EnemyBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();
    m_image = owner->GetComponent<ImageComponent>();

    ParticleM_RegisterEmitter("EnemyDeath");
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

    float deltaTime = FPS_GetDeltaTime();

    // プレイヤー方向
    XMFLOAT3 dirToPlayer = {
        m_playerTransform->GetPosition().x - m_transform->GetPosition().x,
        0.0f,
        m_playerTransform->GetPosition().z - m_transform->GetPosition().z
    };
    dirToPlayer = MiMath::Normalize(dirToPlayer);

    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.x += dirToPlayer.x * 20.0f * deltaTime;
    velocity.z += dirToPlayer.z * 20.0f * deltaTime;
    velocity.x = std::clamp(velocity.x, -3.0f, 3.0f);
    velocity.z = std::clamp(velocity.z, -3.0f, 3.0f);

    velocity.x += m_force.x;
    velocity.y += m_force.y;
    velocity.z += m_force.z;

    m_rigidbody->SetVelocity(velocity);

    // 力をリセットする
    m_force = { 0.0f,0.0f,0.0f };

    // プレイヤーに触れた場合
    for(int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto collisionData = m_collider->GetCollisionData(i);

        if(collisionData.GetCollisionEnter()) {
            GameObject* otherObj = collisionData.m_other->GetOwner();
            if(otherObj->GetName() == "Ball") {
                // ダメージ処理など
                GetOwner()->Destroy();

                // パーティクル発生(test)
                Particle::Data particleData = {};
                particleData.position = m_transform->GetPosition();
                particleData.scaling = { 0.1f,0.1f,0.1f };
                particleData.color = { 0.0f, 1.0f, 0.5f, 1.0f };
                particleData.uvRect = { 0.0f, 0.0f, 1.0f, 1.0f };

                Particle::Settings particleSettings = {};
                particleSettings.velocity = { 0.0f, 5.0f, 0.0f };
                particleSettings.gravity = { 0.0f, -9.8f, 0.0f };
                particleSettings.fadeSize = true;
                particleSettings.fadeAlpha = false;

                ParticleEmit::EmitExplosion(
                    "EnemyDeath",
                    particleData,
                    particleSettings,
                    5.0f,
                    50
                );

                // ボールへ反発処理を送る
                BallBehavior* ballBehavior = otherObj->GetBehavior<BallBehavior>();
                if (ballBehavior) {
                    XMFLOAT3 mtv = collisionData.m_mtv;
                    mtv = MiMath::Normalize(mtv);
                    mtv.x *= -10.0f;
                    mtv.y *= -10.0f;
                    mtv.z *= -10.0f;
                    ballBehavior->AddBounceVelocity(mtv);

                    ballBehavior->AddRadius(0.1f);
                }
            }
        }
    }
}