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
#include "mi_fps.h"
#include "mouse.h"
#include "keyboard.h"

#include "transform_component.h"
#include "camera_component.h"
#include "image_component.h"

TpsCameraBehavior::TpsCameraBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<TpsCameraBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_camera = owner->GetComponent<CameraComponent>();

    m_targetTransform = nullptr;

    m_cameraImageComp = nullptr;
}

TpsCameraBehavior::~TpsCameraBehavior()
{

}

void TpsCameraBehavior::Update(IScene* pScene)
{
    //-------------------------------
    // ターゲットが未設定ならPlayerを探す
    //-------------------------------
    if (!m_targetTransform) {
        GameObject* player = pScene->GetGameObjectByName("Player");
        if (!player) return;

        // ターゲット設定
        m_targetTransform = player->GetComponent<TransformComponent>();

        // 初期位置設定
        m_cameraAnchor = m_targetTransform->GetPosition();
        m_cameraPos = {
            m_cameraAnchor.x,
            m_cameraAnchor.y,
            m_cameraAnchor.z - m_distance,
        };
        return;
    }

    // deltaTime取得
    float deltaTime = FPS_GetDeltaTime();

    //-------------------------------
    // 注視点更新
    //-------------------------------
    XMFLOAT3 targetPos = m_targetTransform->GetPosition();
    targetPos.y += 1.0f; // 少し上を見るようにする
    m_cameraAnchor = MiMath::Lerp(m_cameraAnchor, targetPos, deltaTime * 3.0f);

    float grayRate = m_camera->GetShaderGrayRate();

    // 左クリック中のマウス移動でカメラ回転
    if (Mouse_IsButtonDown(Mouse_Button::LEFT)) {
        float moveX = (float)Mouse_GetPositionX() - (float)Mouse_GetOldPositionX();
        float moveY = (float)Mouse_GetPositionY() - (float)Mouse_GetOldPositionY();

        m_angleX += moveX * deltaTime * 0.1f;
        m_angleY += moveY * deltaTime * 0.1f;

    }

    // スローモーション
    if (Keyboard_IsKeyDown(KK_LEFTSHIFT)) {
        FPS_SetTimeScale(0.3f);
        grayRate = MiMath::Lerp(grayRate, 1.0f, deltaTime * 5.0f);
    }
    else {
        FPS_SetTimeScale(1.0f);
        grayRate = MiMath::Lerp(grayRate, 0.0f, deltaTime * 3.0f);
    }

    m_camera->SetShaderGrayRate(grayRate);

    //-------------------------------
    // カメラ位置設定
    //-------------------------------
    XMFLOAT3 offset = {
        0.0f,
        0.0f,
        -m_distance,
    };

    // 回転
    XMFLOAT3 rotate = { m_angleY, m_angleX, 0.0f };
    offset = MiMath::RotateVector(rotate, offset);
    
    // 補間してなめらかに移動
    XMFLOAT3 desiredCameraPos = {
        m_cameraAnchor.x + offset.x,
        m_cameraAnchor.y + offset.y,
        m_cameraAnchor.z + offset.z,
    };
    m_cameraPos = MiMath::Lerp(m_cameraPos, desiredCameraPos, deltaTime * 4.0f);

    // コンポーネントに反映
    m_camera->SetAtPosition(m_cameraAnchor);
    m_transform->SetPosition(m_cameraPos);

    // imageCompに反映
    if (!m_cameraImageComp) {
        GameObject* cameraImageObj = pScene->GetGameObjectByName("CameraImage");
        if (cameraImageObj) {
            m_cameraImageComp = cameraImageObj->GetComponent<ImageComponent>();
        }
        return;
    }

    m_cameraImageComp->SetTexture(m_camera->GetSnapshot());
}

// カメラの前方向ベクトル取得
XMFLOAT3 TpsCameraBehavior::GetCameraFoward()
{
    // AtPosition - Position
    XMFLOAT3 at = m_camera->GetAtPosition();
    XMFLOAT3 pos = m_transform->GetPosition();

    XMFLOAT3 forward = {
        at.x - pos.x,
        at.y - pos.y,
        at.z - pos.z,
    };
    forward = MiMath::Normalize(forward);

    return forward;
}