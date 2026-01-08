//===================================================
// woodbox_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2026/01/08
//===================================================
#include "woodbox_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "transform_component.h"
#include "rigidbody_component.h"
#include "collider_component.h"
#include "model_component.h"

#include "particle_manager.h"

WoodboxBehavior::WoodboxBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<WoodboxBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_collider = owner->GetComponent<BoxColliderComponent>();
    m_model = owner->GetComponent<ModelComponent>();

    ParticleM_RegisterEmitter("WoodboxBreak");
}

WoodboxBehavior::~WoodboxBehavior()
{

}

void WoodboxBehavior::Update(IScene* pScene)
{

}

void WoodboxBehavior::Break(XMFLOAT3 hitPoint)
{
    if (m_isBroken)return;
    m_isBroken = true;

    // 木箱破壊エフェクト
    Particle::Data data = {};
    data.color = m_model->GetColor();
    data.scaling = { 0.5f,0.5f,0.5f };

    Particle::Settings settings = {};
    settings.gravity = { 0.0f,-9.8f,0.0f };
    settings.fadeSize = true;
    settings.fadeAlpha = false;

    for (int x = 0; x < 6; x++) {
        for (int y = 0; y < 6; y++) {
            for (int z = 0; z < 6; z++) {
                data.position = {
                    m_transform->GetPosition().x + (x - 3) * 0.1f,
                    m_transform->GetPosition().y + (y - 3) * 0.1f,
                    m_transform->GetPosition().z + (z - 3) * 0.1f,
                };
                settings.velocity = {
                    (data.position.x - hitPoint.x) * 5.0f,
                    (data.position.y - hitPoint.y) * 5.0f + 3.0f,
                    (data.position.z - hitPoint.z) * 5.0f,
                };
                ParticleEmit::Emit("WoodboxBreak", data, settings, 10.0f);
            }
        }
    }

    // 破壊処理
    GetOwner()->Destroy();
}
