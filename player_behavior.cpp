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
    m_collider = owner->GetComponent<BoxColliderComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
}

PlayerBehavior::~PlayerBehavior()
{

}

void PlayerBehavior::Update()
{
    // 移動
    DirectX::XMFLOAT3 position = m_transform->GetPosition();
    DirectX::XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.x = 0.0f;
    velocity.z = 0.0f;
    if (Keyboard_IsKeyDown(KK_W)) {
        //position.z += 0.03f;
        velocity.z = 5.0f;
        
    }
    if (Keyboard_IsKeyDown(KK_S)) {
        //position.z -= 0.03f;
        velocity.z = -5.0f;
    }
    if (Keyboard_IsKeyDown(KK_D)) {
        //position.x += 0.03f;
        velocity.x = 5.0f;
    }
    if (Keyboard_IsKeyDown(KK_A)) {
        //position.x -= 0.03f;
        velocity.x = -5.0f;
    }
    //m_transform->SetPosition(position);
    

    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y = 5.0f;
    }

    m_rigidbody->SetVelocity(velocity);


    static float time = 0.0f;
    time += 0.1f;
    if (time > 10.0f) {
        this->GetOwner()->Destroy();
    }
}
