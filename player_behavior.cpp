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
        return;
    }

    // deltaTime取得
    float deltaTime = FPS_GetDeltaTime();

    //-------------------------------
    // 入力処理
    //-------------------------------
    bool isMove = false;

    DirectX::XMFLOAT3 cameraForward = m_tpsCamera->GetCameraFoward();
    float vertical = 0.0f;
    float horizontal = 0.0f;

    // 移動方向ベクトル計算
    if (Keyboard_IsKeyDown(KK_W)) {
        vertical = 1.0f;
        isMove = true;
    }
    if (Keyboard_IsKeyDown(KK_S)) {
        vertical = -1.0f;
        isMove = true;
    }
    if (Keyboard_IsKeyDown(KK_D)) {
        horizontal = 1.0f;
        isMove = true;
    }
    if (Keyboard_IsKeyDown(KK_A)) {
        horizontal = -1.0f;
        isMove = true;
    }

    // カメラの向きに合わせて移動方向を計算
    XMFLOAT3 forward = cameraForward;
    XMFLOAT3 right = { cameraForward.z, 0.0f, -cameraForward.x };

    XMFLOAT3 moveDir = {
        forward.x * vertical + right.x * horizontal,
        0.0f,
        forward.z * vertical + right.z * horizontal,
    };

    // 正規化
    moveDir = MiMath::Normalize(moveDir);

    // 速度設定
    DirectX::XMFLOAT3 velocity = m_rigidbody->GetVelocity();
    if (isMove){
        velocity.x += moveDir.x * 10.0f * deltaTime;
        if (velocity.x > 5.0f)velocity.x = 5.0f;
        else if (velocity.x < -5.0f)velocity.x = -5.0f;

        velocity.z += moveDir.z * 10.0f * deltaTime;
        if (velocity.z > 5.0f)velocity.z = 5.0f;
        else if (velocity.z < -5.0f)velocity.z = -5.0f;
    }

    // ジャンプ
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y += 5.0f;
    }

    //-------------------------------
    // 回転処理
    //-------------------------------
    float speed = MiMath::Length(XMFLOAT3(
        velocity.x,
        0.0f,
        velocity.z
        ))* deltaTime * 100.0f; // 回転速度調整用

    if (speed > 0.1f){
        XMVECTOR quaternion = m_transform->GetRotation();

        // 回転軸
        XMVECTOR    vec1, vec2;
        vec1 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向
        vec2 = XMLoadFloat3(&velocity);             // 進行方向
        vec2 = XMVector3Normalize(vec2);
        XMVECTOR axis = XMVector3Cross(vec1, vec2); // 外積で回転軸を求める

        // 回転量
        XMVECTOR    qu;
        qu = XMQuaternionRotationAxis(
            axis, XMConvertToRadians(speed));
        quaternion = XMQuaternionMultiply(quaternion, qu);

        m_transform->SetRotation(quaternion);
    }

    // 適用処理
    m_rigidbody->SetVelocity(velocity);

    //-------------------------------
    // キャラの更新
    //-------------------------------
    XMFLOAT3 charaPos = m_transform->GetPosition();
    charaPos.y += 2.0f; // キャラの高さ調整
    m_charaTransform->SetPosition(charaPos);

    XMFLOAT3 lookAt = m_tpsCamera->GetCameraFoward();
    lookAt.y = 0.0f;
    float angleY = std::atan2(lookAt.x, lookAt.z);
    m_charaTransform->SetEulerRotation(XMFLOAT3(0.0f, angleY + XM_2PI * 0.5f, 0.0f));

    // キャラの画像切り替え
    int animIndex = 0;
    m_charaAnimTimer += deltaTime;
    animIndex = (int)(m_charaAnimTimer * 7.0f) % 4;
    if (animIndex == 3) animIndex = 1; // 0,1,2のループにする

    int directIndex = 0;
    {
        float dot = MiMath::Dot(lookAt, velocity);
        XMFLOAT3 right = { lookAt.z, 0.0f, -lookAt.x };
        float rightDot = MiMath::Dot(right, velocity);
        if (abs(dot) > abs(rightDot)) {
                    // 前後方向
            if (dot >= 0.1f) {
                // 前方向
                directIndex = 3;
            }
            else if(dot <= -0.1f){
                directIndex = 0;
            }
        }
        else {
            // 左右方向
            if (rightDot >= 0.1f) {
                // 右方向
                directIndex = 2;
            }
            else if(rightDot <= -0.1f){
                directIndex = 1;
            }
        }
    }

    m_charaImage->SetUvRect({1.0f / 3.0f * (float)animIndex, 1.0f / 4.0f * (float)directIndex, 1.0f / 3.0f, 1.0f / 4.0f});
}
