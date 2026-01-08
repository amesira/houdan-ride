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

    m_tpsCamera = nullptr;
}

PlayerBehavior::~PlayerBehavior()
{

}

void PlayerBehavior::Update(IScene* pScene)
{
    // 参考オブジェクト取得
    GetReferenceObjects(pScene);
    if (!m_tpsCamera || !m_charaTransform) return;

    // switch_sprite_behaviorへ設定
    SwitchSpriteBehavior* switchSpriteBe = GetOwner()->GetBehavior<SwitchSpriteBehavior>();
    if (switchSpriteBe) {
        switchSpriteBe->SetImageComponent(m_charaImage);
    }

    // deltaTime取得
    float deltaTime = FPS_GetDeltaTime();

    // 移動処理更新
    UpdateMovement(deltaTime);

    // ボール回転更新
    UpdateBallRotation(deltaTime);

    // キャラクター更新
    UpdateCharacter(deltaTime);
}

// 参考オブジェクトの取得
void PlayerBehavior::GetReferenceObjects(IScene* pScene)
{
    // charaの参照取得
    if (!m_charaTransform) {
        GameObject* charaObj = pScene->GetGameObjectByName("Player_Chara");
        if (charaObj) {
            m_charaTransform = charaObj->GetComponent<TransformComponent>();
            m_charaImage = charaObj->GetComponent<ImageComponent>();
        }
    }

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

    velocity.x += m_bounceVelocity.x;
    velocity.y += m_bounceVelocity.y;
    velocity.z += m_bounceVelocity.z;
    m_bounceVelocity = { 0.0f,0.0f,0.0f };

    // 適用処理
    m_rigidbody->SetVelocity(velocity);
}

// ボール回転の更新
void PlayerBehavior::UpdateBallRotation(float deltaTime)
{
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    velocity.y = 0.0f;

    // 回転量計算
    XMFLOAT3 prevDiff = {
        m_transform->GetPosition().x - m_transform->GetPrevPosition().x,
        0.0f,
        m_transform->GetPosition().z - m_transform->GetPrevPosition().z,
    };
    float speed = MiMath::Length(prevDiff) * 100.0f;

    // 回転適用
    if (speed > 0.1f) {
        XMVECTOR quaternion = m_transform->GetRotation();

        // 回転軸
        XMVECTOR    vec1, vec2;
        vec1 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向
        vec2 = XMLoadFloat3(&velocity);             // 進行方向
        vec2 = XMVector3Normalize(vec2);
        XMVECTOR axis = XMVector3Cross(vec1, vec2); // 外積で回転軸を求める
        if (XMVectorGetX(XMVector3LengthSq(axis)) < 0.0001f) {
            axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // 進行方向が上方向と同じ場合はX軸を回転軸にする
        }

        // 回転量
        XMVECTOR    qu;
        qu = XMQuaternionRotationAxis(
            axis, XMConvertToRadians(speed));
        quaternion = XMQuaternionMultiply(quaternion, qu);

        m_transform->SetRotation(quaternion);
    }
}

void PlayerBehavior::UpdateCharacter(float deltaTime)
{
    //-------------------------------
    // キャラクターの位置更新
    //-------------------------------
    XMFLOAT3 charaPos = {
        m_transform->GetPosition().x,
        m_transform->GetPosition().y + 2.0f,
        m_transform->GetPosition().z,
    };
    m_charaTransform->SetPosition(charaPos);
}

void PlayerBehavior::AddReflection(XMFLOAT3 mtv)
{
    XMFLOAT3 velocity = m_rigidbody->GetVelocity();

    // 法線ベクトル計算
    XMFLOAT3 normal = MiMath::Normalize(mtv);
    //if (fabsf(normal.y) < 0.5f) return;

    

    // 速度ベクトルを法線ベクトルに投影
    float velocityDotNormal = MiMath::Dot(velocity, normal);
    if (velocityDotNormal < 0.0f) velocityDotNormal = -velocityDotNormal;
    XMFLOAT3 projectedVelocity = {
        normal.x * velocityDotNormal,
        normal.y * velocityDotNormal,
        normal.z * velocityDotNormal,
    };
    // 跳ね返りベクトル計算
    XMFLOAT3 bounceVelocity = {
        projectedVelocity.x * -3.5f,
        projectedVelocity.y * -1.5f,
        projectedVelocity.z * -3.5f,
    };
    // 速度に跳ね返りを加算
    m_bounceVelocity.x += bounceVelocity.x;
    m_bounceVelocity.y += bounceVelocity.y;
    m_bounceVelocity.z += bounceVelocity.z;
}
