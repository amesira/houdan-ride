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

    m_childTransforms.clear();
    m_childColliders.clear();

    for(int i = 0; i < 32; i++) {
        m_rideTransform[i] = nullptr;
    }
}

TrainBehavior::~TrainBehavior()
{

}

void TrainBehavior::Update(IScene* pScene)
{
    float deltaTime = FPS_GetDeltaTime();

    XMFLOAT3 addPos = { 0.0f,0.0f,0.0f };
    // 列車を前進させる
    addPos.z += m_moveSpeed * deltaTime;
    
    // 上下させる
    m_timer += deltaTime;
    addPos.y += std::sinf(m_timer * 3.0f) * 0.5f * deltaTime;

    m_transform->SetPosition({
        m_transform->GetPosition().x + addPos.x,
        m_transform->GetPosition().y + addPos.y,
        m_transform->GetPosition().z + addPos.z,
        });
    
    // 乗っている物体を取得
    EntryRideObjects(m_collider);
    for (auto& childCol : m_childColliders) {
        EntryRideObjects(childCol);
    }

    // 乗っている物体を同じ速度で動かす
    for(int i = 0; i < 32; i++) {
        if (m_rideTransform[i] == nullptr)continue;

        m_rideTransform[i]->SetPosition({
            m_rideTransform[i]->GetPosition().x + addPos.x,
            m_rideTransform[i]->GetPosition().y + addPos.y,
            m_rideTransform[i]->GetPosition().z + addPos.z,
            });
        m_rideTimer[i] -= FPS_GetDeltaTime();
        if (m_rideTimer[i] <= 0.0f){
            m_rideTransform[i] = nullptr;
        }
    }

    // 子オブジェクトを同じ位置へ
    for (auto& childTransform : m_childTransforms) {
        childTransform->SetPosition(m_transform->GetPosition());
    }
}

void TrainBehavior::EntryRideObjects(BoxColliderComponent* col)
{
    // 乗っている物体を取得して同じ速度で動かす
    for (int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto collisionData = col->GetCollisionData(i);
        if (collisionData.GetCollisionStay()) {
            GameObject* otherObj = collisionData.m_other->GetOwner();
            if (otherObj->GetName() == "Train" || otherObj->GetName() == "TrainChildCollider")continue; // 自分自身は無視

            auto* otherTransform = otherObj->GetComponent<TransformComponent>();

            // 乗っている物体を列車と同じ速度で動かす
            if (!otherTransform)continue;
            if (collisionData.m_mtv.y > 0.0f) {
                // 下から突き抜けた場合は無視
                continue;
            }

            // 乗っている物体リストに登録
            bool alreadyRegistered = false;
            for(int j = 0; j < 32; j++) {
                if (m_rideTransform[j] == otherTransform) {
                    alreadyRegistered = true;
                    m_rideTimer[j] = 0.5f; // タイマーリセット
                }
            }
            if (alreadyRegistered)continue;

            for(int i = 0; i < 32; i++) {
                if (m_rideTransform[i] == nullptr) {
                    m_rideTransform[i] = otherTransform;
                    m_rideTimer[i] = 0.5f;
                    break;
                }
            }
        }
    }
}

void TrainBehavior::AddChildCollider(GameObject* collider)
{
    BoxColliderComponent* colComp = collider->GetComponent<BoxColliderComponent>();
    m_childColliders.push_back(colComp);
    TransformComponent* childTransform = collider->GetComponent<TransformComponent>();
    m_childTransforms.push_back(childTransform);

    collider->SetName("TrainChildCollider");
}

XMFLOAT3 TrainBehavior::GetPosition() const
{
    return m_transform->GetPosition();
}
