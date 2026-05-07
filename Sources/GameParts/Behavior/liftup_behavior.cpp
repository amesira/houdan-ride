#include "liftup_behavior.h"
#include "Sources/Scene/scene_interface.h"
#include "Sources/Core/game_object.h"
#include "Sources/Core/type_id.h"

#include "Sources/GameParts/Component/transform_component.h"
#include "Sources/GameParts/Component/rigidbody_component.h"

#include "Sources/System/mi_fps.h"
#include "Utility/mi_math.h"

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
