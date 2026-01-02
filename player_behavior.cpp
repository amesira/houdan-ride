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

#include "tps_camera_behavior.h"

#include "keyboard.h"
#include "fps.h"

PlayerBehavior::PlayerBehavior(GameObject* owner) 
    : Behavior(BehaviorTypeID::getTypeID<PlayerBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_cubemesh = owner->GetComponent<CubemeshComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();

    m_cubemesh->SetEnable(false);

    m_tpsCamera = nullptr;
}

PlayerBehavior::~PlayerBehavior()
{

}

void PlayerBehavior::Update(IScene* pScene)
{
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
        velocity = {
            moveDir.x * 5.0f,
            m_rigidbody->GetVelocity().y,
            moveDir.z * 5.0f,
        };
    }

    // ジャンプ
    if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
        velocity.y += 5.0f;
    }

    //-------------------------------
    // 回転処理
    //-------------------------------
    //XMFLOAT3 rotation = m_transform->GetRotation();

    if (abs(velocity.x) > 0.01f || abs(velocity.z) > 0.01f) {
        //// 速度（水平のみ）
        //XMVECTOR v = XMLoadFloat3(&velocity);
        //v = XMVectorSet(XMVectorGetX(v), 0.0f, XMVectorGetZ(v), 0.0f);

        //float speed = XMVectorGetX(XMVector3Length(v));
        //if (speed < 0.01f) return;

        //XMVECTOR dir = XMVector3Normalize(v);

        //// 転がり軸： up × dir
        //XMVECTOR up = XMVectorSet(0, 1, 0, 0);
        //XMVECTOR axis = XMVector3Cross(up, dir);

        //float axisLen = XMVectorGetX(XMVector3Length(axis));
        //if (axisLen < 1e-6f) return;
        //axis = axis / axisLen;

        //// 回転角（rad）= 移動距離 / 半径
        //float dist = speed * deltaTime;
        //float angle = dist / 0.5f;

        //// このフレームの回転
        //XMVECTOR dq = XMQuaternionRotationAxis(axis, angle);

        //// 合成（まずはワールド回転として積む）
        //XMVECTOR q = m_quaternion;
        //q = XMQuaternionNormalize(XMQuaternionMultiply(dq, q));
        //m_quaternion = q;
    }
    
    // 適用処理
    m_rigidbody->SetVelocity(velocity);
    //m_transform->SetRotation(rotation);
}
