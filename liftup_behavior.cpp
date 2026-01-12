#include "liftup_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"

#include "transform_component.h"
#include "rigidbody_component.h"

#include "mi_fps.h"
#include "mi_math.h"

LiftupBehavior::LiftupBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<LiftupBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    targetPos = m_transform->GetPosition();
    targetPos.y += 3.0f;
}

LiftupBehavior::~LiftupBehavior()
{

}

void LiftupBehavior::Update(IScene* pScene)
{
    XMFLOAT3 pos = MiMath::Lerp(m_transform->GetPosition(), targetPos, FPS_GetDeltaTime() * 2.0f);
    m_transform->SetPosition(pos);
}
