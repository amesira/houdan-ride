//===================================================
// train_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/01/09
//===================================================
#include "train_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "mi_math.h"
#include "mi_fps.h"

#include "transform_component.h"
#include "rigidbody_component.h"
#include "collider_component.h"
#include "model_component.h"

TrainBehavior::TrainBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<TrainBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_collider = owner->GetComponent<BoxColliderComponent>();
    m_model = owner->GetComponent<ModelComponent>();
}

TrainBehavior::~TrainBehavior()
{

}

void TrainBehavior::Update(IScene* pScene)
{
    // 列車を前進させる
    m_transform->SetPosition({
        m_transform->GetPosition().x,
        m_transform->GetPosition().y,
        m_transform->GetPosition().z + m_moveSpeed * FPS_GetDeltaTime(),
        });

    // 乗っている物体を取得して同じ速度で動かす
    for (int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto collisionData = m_collider->GetCollisionData(i);
        if (collisionData.GetCollisionStay()) {
            GameObject* otherObj = collisionData.m_other->GetOwner();
            auto* otherTransform = otherObj->GetComponent<TransformComponent>();

            // 乗っている物体を列車と同じ速度で動かす
            if (!otherTransform)continue;
            if (collisionData.m_mtv.y > 0.0f) {
                // 下から突き抜けた場合は無視
                continue;
            }

            otherTransform->SetPosition({
                otherTransform->GetPosition().x,
                otherTransform->GetPosition().y,
                otherTransform->GetPosition().z + m_moveSpeed * FPS_GetDeltaTime(),
                });
        }
    }
}
