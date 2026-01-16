#include "field_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "mi_fps.h"
#include "mi_math.h"

#include "transform_component.h"

FieldBehavior::FieldBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<FieldBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_shipTransform = nullptr;
}

FieldBehavior::~FieldBehavior()
{
}

void FieldBehavior::Update(IScene* pScene)
{
    if (m_shipTransform == nullptr) {
        GameObject* shipObj = pScene->GetGameObjectByName("Train");
        if (shipObj) {
            m_shipTransform = shipObj->GetComponent<TransformComponent>();
        }
        return;
    }
    else {
        if (m_transform->GetPosition().z < m_shipTransform->GetPosition().z - 50.0f) {
            // 船から遠く離れたら消す
            GetOwner()->Destroy();
            return;
        }
    }

    if(m_transform->GetPosition().y < -50.0f) {
        // 一定以下に落ちたら消す
        GetOwner()->Destroy();
        return;
    }
}