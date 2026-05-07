#include "golf_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "mi_math.h"
#include "mi_fps.h"

#include "debug_ostream.h"

#include "transform_component.h"
#include "collider_component.h"

#include "particle_manager.h"

GolfBehavior::GolfBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<GolfBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_collider = owner->GetComponent<BoxColliderComponent>();

    m_isCleared = false;

    ParticleM_RegisterEmitter("ExpEffect");
}

GolfBehavior::~GolfBehavior()
{

}

void GolfBehavior::Update(IScene* pScene)
{
    if (m_isCleared)return;

    for (int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto colData = m_collider->GetCollisionData(i);
        if(colData.GetCollisionEnter()) {
            GameObject* otherObj = colData.m_other->GetOwner();
            if (otherObj->GetName() == "Ball") {
                hal::dout << "Goal!" << std::endl;
                m_isCleared = true;

                // 木箱破壊エフェクト
                Particle::Data data = {};
                data.color = { 1.0f, 1.0f,0.0f,1.0f };
                data.scaling = { 0.2f, 0.2f, 0.2f };
                data.uvRect = { 0.0f,0.0f,1.0f,1.0f };

                Particle::Settings settings = {};
                settings.gravity = { 0.0f,-9.8f,0.0f };
                settings.fadeSize = true;
                settings.fadeAlpha = false;

                XMFLOAT3 position = m_transform->GetPosition();

                for (int x = 0; x < 6; x++) {
                    for (int y = 0; y < 6; y++) {
                        for (int z = 0; z < 6; z++) {
                            data.position = {
                                m_transform->GetPosition().x + (2 - x) * m_transform->GetScaling().x / 4.0f,
                                m_transform->GetPosition().y + (2 - y) * m_transform->GetScaling().y / 4.0f,
                                m_transform->GetPosition().z + (2 - z) * m_transform->GetScaling().z / 4.0f,
                            };
                            settings.velocity = {
                                (data.position.x - position.x) * static_cast<float>(rand() % 100) / 100.0f * 10.0f,
                                (data.position.y - position.y) * static_cast<float>(rand() % 100) / 100.0f * 10.0f,
                                (data.position.z - position.z) * static_cast<float>(rand() % 100) / 100.0f * 10.0f,
                            };
                            ParticleEmit::Emit("ExpEffect", data, settings, 10.0f);

                        }
                    }
                }

                break;
            }
        }
    }
}

