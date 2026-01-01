//===================================================
// tps_camera_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/12/31
//===================================================
#include "tps_camera_behavior.h"

#include "scene_interface.h"
#include "game_object.h"

#include "mi_math.h"
#include "fps.h"
#include "mouse.h"

#include "transform_component.h"
#include "camera_component.h"

TpsCameraBehavior::TpsCameraBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<TpsCameraBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_camera = owner->GetComponent<CameraComponent>();

    m_targetTransform = nullptr;
}

TpsCameraBehavior::~TpsCameraBehavior()
{

}

void TpsCameraBehavior::Update(IScene* pScene)
{
    if (!m_targetTransform) {
        GameObject* player = pScene->GetGameObjectByName("Player");
        if (!player) return;
        m_targetTransform = player->GetComponent<TransformComponent>();
        return;
    }

    float deltaTime = FPS_GetDeltaTime();

    // 注視点設定
    XMFLOAT3 targetPos = m_targetTransform->GetPosition();
    targetPos.y += 1.0f; // 少し上を見るようにする
    m_cameraAnchor = MiMath::Lerp(m_cameraAnchor, targetPos, deltaTime * 3.0f);

    // カメラ位置設定
    XMFLOAT3 desiredCameraPos = {
        m_cameraAnchor.x,
        m_cameraAnchor.y + 2.0f,
        m_cameraAnchor.z - 5.0f
    };

    m_camera->SetAtPosition(m_cameraAnchor);
}
