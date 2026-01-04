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

#include "mi_math.h"

#include "transform_component.h"
#include "cubemesh_component.h"
#include "collider_component.h"
#include "rigidbody_component.h"
#include "image_component.h"

#include "tps_camera_behavior.h"

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
        velocity.x += moveDir.x * 10.0f * deltaTime;
        velocity.z += moveDir.z * 10.0f * deltaTime;

        velocity.x = std::clamp(velocity.x, -5.0f, 5.0f);
        velocity.z = std::clamp(velocity.z, -5.0f, 5.0f);
    }

    // ジャンプ
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y += 5.0f;
    }

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

    //-------------------------------
    // HD2D画像更新
    //-------------------------------
    XMFLOAT3 lookAt = m_tpsCamera->GetCameraFoward();
    lookAt.y = 0.0f;
    float angleY = std::atan2(lookAt.x, lookAt.z);
    m_charaTransform->SetEulerRotation(XMFLOAT3(0.0f, angleY + XM_2PI * 0.5f, 0.0f));

    //-------------------------------
    // キャラの画像切り替え
    //-------------------------------
    static const float CHARA_SPRITE_WIDTH = 1.0f / 3.0f;
    static const float CHARA_SPRITE_HEIGHT = 1.0f / 4.0f;

    m_charaAnimTimer += deltaTime;

    // 連番インデックス計算（0~2を繰り返す）
    int sequenceIndex = (int)(m_charaAnimTimer * 7.0f) % 4;
    if (sequenceIndex == 3) sequenceIndex = 1;

    // 方向インデックス計算
    int directIndex = 0;

    XMFLOAT3 cameraForward = { m_tpsCamera->GetCameraFoward().x , 0.0f, m_tpsCamera->GetCameraFoward().z };
    XMFLOAT3 cameraRight = { cameraForward.z, 0.0f, -cameraForward.x };
    
    float forwardDot = MiMath::Dot(cameraForward, m_rigidbody->GetVelocity());
    float rightDot = MiMath::Dot(cameraRight, m_rigidbody->GetVelocity());

    // 前後方向
    if (fabsf(forwardDot) > fabsf(rightDot)) {
        if (forwardDot >= 0.1f)directIndex = 3;         // 前方向（背を向ける）
        else if (forwardDot <= -0.1f)directIndex = 0;   // 後方向（正面）
    }
    // 左右方向
    else {
        if (rightDot >= 0.1f) directIndex = 2;          // 右方向
        else if (rightDot <= -0.1f) directIndex = 1;    // 左方向
    }

    // UV矩形設定
    m_charaImage->SetUvRect({
        CHARA_SPRITE_WIDTH * (float)sequenceIndex,
        CHARA_SPRITE_HEIGHT * (float)directIndex,
        CHARA_SPRITE_WIDTH,
        CHARA_SPRITE_HEIGHT});

}
