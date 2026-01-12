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

#include "mi_math.h"

#include "transform_component.h"
#include "rigidbody_component.h"
#include "collider_component.h"
#include "model_component.h"

#include "particle_manager.h"

#include "mi_fps.h"

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
    if (m_isBroken) {
        m_breakTimer += FPS_GetDeltaTime();

        //if (m_breakTimer > 0.3f) {
            XMFLOAT3 mtv = m_breakMtv;

            XMFLOAT3 hitPoint = {
               m_transform->GetPosition().x + mtv.x * m_transform->GetScaling().x / 2.0f,
               m_transform->GetPosition().y + mtv.y * m_transform->GetScaling().y / 2.0f,
               m_transform->GetPosition().z + mtv.z * m_transform->GetScaling().z / 2.0f,
                    };

            // 木箱破壊エフェクト
            Particle::Data data = {};
            data.color = m_model->GetColor();
            data.scaling = { 0.3f,0.3f,0.3f };

            Particle::Settings settings = {};
            settings.gravity = { 0.0f,-9.8f,0.0f };
            settings.fadeSize = true;
            settings.fadeAlpha = false;

            for (int x = 0; x < 6; x++) {
                for (int y = 0; y < 6; y++) {
                    for (int z = 0; z < 6; z++) {
                        data.position = {
                            m_transform->GetPosition().x + (3 - x) * m_transform->GetScaling().x / 6.0f,
                            m_transform->GetPosition().y + (3 - y) * m_transform->GetScaling().y / 6.0f,
                            m_transform->GetPosition().z + (3 - z) * m_transform->GetScaling().z / 6.0f,
                        };
                        settings.velocity = {
                            (data.position.x - hitPoint.x) * static_cast<float>(rand() % 100) / 100.0f * 10.0f,
                            (data.position.y - hitPoint.y) * static_cast<float>(rand() % 100) / 100.0f * 10.0f,
                            (data.position.z - hitPoint.z) * static_cast<float>(rand() % 100) / 100.0f * 10.0f,
                        };
                        ParticleEmit::Emit("WoodboxBreak", data, settings, 10.0f);
                    }
                }
            }

            // 破壊処理
            GetOwner()->Destroy();
        //}
    }
}

void WoodboxBehavior::Break(XMFLOAT3 mtv)
{
    if (m_isBroken)return;
    m_isBroken = true;

    mtv = MiMath::Normalize(mtv);
    m_breakMtv = mtv;

   
}
