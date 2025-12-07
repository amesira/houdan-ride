//===================================================
// physics_processor.cpp [物理演算制御プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "physics_processor.h"

#include "transform_component.h"
#include "rigidbody_component.h"

#include "fps.h"

void PhysicsProcessor::Initialize()
{
    
}

void PhysicsProcessor::Finalize()
{

}

void PhysicsProcessor::Process()
{
    float deltaTime = FPS_GetDeltaTime();

    for (int i = 0; i < m_components.size(); i++) {
        TransformComponent* transform = m_components[i].m_transform;
        RigidbodyComponent* rigidbody = m_components[i].m_rigidbody;

        transform->SetPrevPosition(transform->GetPosition());
        rigidbody->SetPrevVelocity(rigidbody->GetPrevVelocity());

        DirectX::XMFLOAT3 velocity = rigidbody->GetVelocity();
        {
            // 摩擦の適用
            const DirectX::XMFLOAT3 friction = rigidbody->GetFriction();
            velocity.x *= friction.x;
            velocity.y *= friction.y;
            velocity.z *= friction.z;

            // 重力の適用
            velocity.y += rigidbody->GetGravityScale() * deltaTime;
        }
        rigidbody->SetVelocity(velocity);

        // 位置の更新
        DirectX::XMFLOAT3 position = transform->GetPosition();
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.z += velocity.z * deltaTime;
        transform->SetPosition(position);
    }
}
