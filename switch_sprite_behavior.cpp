// switch_sprite_behavior.h
#include "switch_sprite_behavior.h"
#include "scene_interface.h"
#include "game_object.h"
#include "type_id.h"
using namespace DirectX;

#include "mi_fps.h"
#include "mi_math.h"
#include "tps_camera_behavior.h"

#include "image_component.h"
#include "rigidbody_component.h"

SwitchSpriteBehavior::SwitchSpriteBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<SwitchSpriteBehavior>())
{
    m_image = owner->GetComponent<ImageComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();

    m_frameTimer = 0.0f;
}

SwitchSpriteBehavior::~SwitchSpriteBehavior()
{

}

void SwitchSpriteBehavior::Update(IScene* pScene)
{
    if (!m_image || !m_rigidbody)return;

    // TPSカメラの参照取得
    if (!m_tpsCamera) {
        GameObject* cameraObj = pScene->GetGameObjectByName("TPSCamera");
        if (cameraObj) {
            m_tpsCamera = cameraObj->GetBehavior<TpsCameraBehavior>();
        }
        return;
    }

    const float CHARA_SPRITE_WIDTH = 1.0f / 3.0f;
    const float CHARA_SPRITE_HEIGHT = 1.0f / 4.0f;

    // 速度取得（Y成分は無視）
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.y = 0.0f;

    float speed = MiMath::Length(velocity) * 0.3f;
    if (speed < 1.0f)speed = 1.0f;
    m_frameTimer += FPS_GetDeltaTime() * speed;

    // 連番インデックス計算（0~2を繰り返す）
    int sequenceIndex = (int)(m_frameTimer * 7.0f) % 4;
    if (sequenceIndex == 3) sequenceIndex = 1;

    // 方向インデックス計算
    XMFLOAT3 cameraForward = { m_tpsCamera->GetCameraFoward().x , 0.0f, m_tpsCamera->GetCameraFoward().z };
    XMFLOAT3 cameraRight = { cameraForward.z, 0.0f, -cameraForward.x };

    float forwardDot = MiMath::Dot(cameraForward, velocity);
    float rightDot = MiMath::Dot(cameraRight, velocity);

    // 前後方向
    if (fabsf(forwardDot) > fabsf(rightDot)) {
        if (forwardDot >= 0.1f)m_directIndex = 3;         // 前方向（背を向ける）
        else if (forwardDot <= -0.1f)m_directIndex = 0;   // 後方向（正面）
    }
    // 左右方向
    else {
        if (rightDot >= 0.1f) m_directIndex = 2;          // 右方向
        else if (rightDot <= -0.1f) m_directIndex = 1;    // 左方向
    }

    // UV矩形設定
    m_image->SetUvRect({
        CHARA_SPRITE_WIDTH * (float)sequenceIndex,
        CHARA_SPRITE_HEIGHT * (float)m_directIndex,
        CHARA_SPRITE_WIDTH,
        CHARA_SPRITE_HEIGHT });
}