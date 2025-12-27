//===================================================
// player_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#include "player_behavior.h"

#include "type_id.h"
#include "game_object.h"
#include "transform_component.h"
#include "cubemesh_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"

#include "keyboard.h"

PlayerBehavior::PlayerBehavior(GameObject* owner) 
    : Behavior(BehaviorTypeID::getTypeID<PlayerBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_cubemesh = owner->GetComponent<CubemeshComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();

    //m_rigidbody->SetEnable(false);
    m_cubemesh->SetEnable(false);
}

PlayerBehavior::~PlayerBehavior()
{

}

void PlayerBehavior::Update()
{
    // 移動
    DirectX::XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.x = 0.0f;
    velocity.z = 0.0f;

    if (Keyboard_IsKeyDown(KK_W)) {
        velocity.z = 5.0f;
        
    }
    if (Keyboard_IsKeyDown(KK_S)) {
        velocity.z = -5.0f;
    }
    if (Keyboard_IsKeyDown(KK_D)) {
        velocity.x = 5.0f;
    }
    if (Keyboard_IsKeyDown(KK_A)) {
        velocity.x = -5.0f;
    }

    XMFLOAT3 angle = m_transform->GetRotation();
    if (Keyboard_IsKeyDown(KK_Q)) {
        angle.z -= 0.05f;
    }
    if (Keyboard_IsKeyDown(KK_E)) {
        angle.z += 0.05f;
    }
    m_transform->SetRotation(angle);

    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y += 3.0f;
    }
    if (Keyboard_IsKeyDownTrigger(KK_LEFTSHIFT)) {
        velocity.y -= 3.0f;
    }

   /* XMFLOAT3 position = m_transform->GetPosition();
    position.x += velocity.x * 0.016f;
    position.y += velocity.y * 0.016f;
    position.z += velocity.z * 0.016f;
    m_transform->SetPosition(position);*/
    m_cubemesh->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    
    for(int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        ColliderComponent::CollisionData data = m_collider->GetCollisionData(i);
        if (data.m_other == nullptr) continue;

        if (data.GetCollisionStay()) {
            m_cubemesh->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        }
    }

    m_rigidbody->SetVelocity(velocity);
}
