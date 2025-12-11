//===================================================
// dynamics_processor.cpp [物理演算補正プロセッサー]
// 
// Author：Miu Kitamura
// Date  ：2025/10/28
//===================================================
#include "dynamics_processor.h"

#include "game_object.h"
#include "scene_interface.h"

#include "transform_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"

#include "fps.h"

void DynamicsProcessor::Initialize()
{

}

void DynamicsProcessor::Finalize()
{

}

void DynamicsProcessor::Process(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    auto* rigidbodyPool = pScene->GetComponentPool<RigidbodyComponent>();
    auto* boxColliderPool = pScene->GetComponentPool<BoxColliderComponent>();
    auto* transformPool = pScene->GetComponentPool<TransformComponent>();

    auto& rigidbodyList = rigidbodyPool->GetList();

    for (RigidbodyComponent& r : rigidbodyList) {
        TransformComponent* transform = transformPool->GetByGameObjectID(r.GetOwner()->GetID());
        BoxColliderComponent* collider = boxColliderPool->GetByGameObjectID(r.GetOwner()->GetID());
        RigidbodyComponent* rigidbody = &r;

        bool isGrounded = false;

        // 外力を算出
        DirectX::XMFLOAT3 revVelocity = {0.0f,0.0f,0.0f};
        {
            DirectX::XMFLOAT3   minMtv = collider->GetMinMtv();
            DirectX::XMFLOAT3   maxMtv = collider->GetMaxMtv();

            // minMtvが負、maxMtvが正の成分のみを採用する
            if (minMtv.x > 0.0f)minMtv.x = 0.0f;
            if (minMtv.y > 0.0f)minMtv.y = 0.0f;
            if (minMtv.z > 0.0f)minMtv.z = 0.0f;

            if (maxMtv.x < 0.0f)maxMtv.x = 0.0f;
            if (maxMtv.y < 0.0f)maxMtv.y = 0.0f;
            if (maxMtv.z < 0.0f)maxMtv.z = 0.0f;

            // 外力に適用
            revVelocity.x += (minMtv.x + maxMtv.x) / deltaTime;
            revVelocity.y += (minMtv.y + maxMtv.y) / deltaTime;
            revVelocity.z += (minMtv.z + maxMtv.z) / deltaTime;

            // 接地判定
            if (minMtv.y < 0.03f) {
                isGrounded = true;
            }
        }

        // 外力が大きくなりすぎるのを防ぐ
        //float clamp = 2.0f;
        ///*if (revVelocity.x > clamp)revVelocity.x = clamp;
        //else if (revVelocity.x < -clamp)revVelocity.x = -clamp;*/
        //if (revVelocity.y > clamp)revVelocity.y = clamp;
        //else if (revVelocity.y < -clamp)revVelocity.y = -clamp;
        ///*if (revVelocity.z > clamp)revVelocity.z = clamp;
        //else if (revVelocity.z < -clamp)revVelocity.z = -clamp;*/

        // 位置と速度を算出
        DirectX::XMFLOAT3   position = transform->GetPosition();
        position.x += revVelocity.x * deltaTime;
        position.y += revVelocity.y * deltaTime;
        position.z += revVelocity.z * deltaTime;

        DirectX::XMFLOAT3   velocity = rigidbody->GetVelocity();
        velocity.x += revVelocity.x;
        velocity.y += revVelocity.y;
        velocity.z += revVelocity.z;

        // 動きが小さい場合は、位置を1フレーム前にリセット
       /* const DirectX::XMFLOAT3 prevPosition = transform->GetPrevPosition();
        if (abs(position.x - prevPosition.x) < 0.02f) {
            position.x = prevPosition.x;
            velocity.x *= 0.5f;
        }
        if (abs(position.y - prevPosition.y) < 0.02f) {
            position.y = prevPosition.y;
            velocity.y *= 0.5f;
        }
        if (abs(position.z - prevPosition.z) < 0.02f) {
            position.z = prevPosition.z;
            velocity.z *= 0.5f;
        }*/

        // 適用
        transform->SetPosition(position);
        rigidbody->SetVelocity(velocity);
        rigidbody->SetIsGrounded(isGrounded);
    }
}
