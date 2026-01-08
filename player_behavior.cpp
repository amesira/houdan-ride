//===================================================
// player_behavior.cpp
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//===================================================
#include "player_behavior.h"
#include "scene_interface.h"

#include "type_id.h"
#include "game_object.h"
using namespace DirectX;

#include "debug_ostream.h"

#include "mi_math.h"

#include "transform_component.h"
#include "cubemesh_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"
#include "image_component.h"

#include "tps_camera_behavior.h"
#include "switch_sprite_behavior.h"

#include "keyboard.h"
#include "fps.h"

PlayerBehavior::PlayerBehavior(GameObject* owner) 
    : Behavior(BehaviorTypeID::getTypeID<PlayerBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_image = owner->GetComponent<ImageComponent>();

    m_tpsCamera = nullptr;
}

PlayerBehavior::~PlayerBehavior()
{

}

void PlayerBehavior::Update(IScene* pScene)
{
    // 参考オブジェクト取得
    GetReferenceObjects(pScene);
    if (!m_tpsCamera) return;

    // deltaTime取得
    float deltaTime = FPS_GetDeltaTime();

    // 移動処理更新
    UpdateMovement(deltaTime);
}

// 参考オブジェクトの取得
void PlayerBehavior::GetReferenceObjects(IScene* pScene)
{
    // TPSカメラの参照取得
    if (!m_tpsCamera) {
        GameObject* cameraObj = pScene->GetGameObjectByName("TPSCamera");
        if (cameraObj) {
            m_tpsCamera = cameraObj->GetBehavior<TpsCameraBehavior>();
        }
    }
}

// 移動処理の更新
void PlayerBehavior::UpdateMovement(float deltaTime)
{
    //-------------------------------
    // 入力処理
    //-------------------------------
    float horizontal = 0.0f;
    float vertical = 0.0f;

    // 移動方向ベクトル計算
    if (Keyboard_IsKeyDown(KK_D)) {
        horizontal = 1.0f;
    }
    if (Keyboard_IsKeyDown(KK_A)) {
        horizontal = -1.0f;
    }
    if (Keyboard_IsKeyDown(KK_W)) {
        vertical = 1.0f;
    }
    if (Keyboard_IsKeyDown(KK_S)) {
        vertical = -1.0f;
    }

    //-------------------------------
    // カメラの向きに合わせて移動方向を計算
    //-------------------------------
    XMFLOAT3 cameraForward = m_tpsCamera->GetCameraFoward();
    XMFLOAT3 cameraRight = { cameraForward.z, 0.0f, -cameraForward.x };

    XMFLOAT3 moveDir = {
        cameraForward.x * vertical + cameraRight.x * horizontal,
        0.0f,
        cameraForward.z * vertical + cameraRight.z * horizontal,
    };

    // 正規化
    moveDir = MiMath::Normalize(moveDir);

    //-------------------------------
    // 速度設定
    //-------------------------------
    DirectX::XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    // 入力がある場合のみ加速
    if (fabsf(horizontal) > 0.1f || fabsf(vertical) > 0.1f) {
        velocity.x += moveDir.x * 20.0f * deltaTime;
        velocity.z += moveDir.z * 20.0f * deltaTime;

        velocity.x = std::clamp(velocity.x, -5.0f, 5.0f);
        velocity.z = std::clamp(velocity.z, -5.0f, 5.0f);
    }

    // ジャンプ
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y += 9.0f;
    }

    // 適用処理
    m_rigidbody->SetVelocity(velocity);
}