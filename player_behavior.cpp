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
#include "slider_component.h"

#include "tps_camera_behavior.h"
#include "switch_sprite_behavior.h"
#include "ball_behavior.h"

#include "keyboard.h"
#include "mi_fps.h"

#include "particle_manager.h"
#include "text_component.h"

PlayerBehavior::PlayerBehavior(GameObject* owner) 
    : Behavior(BehaviorTypeID::getTypeID<PlayerBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_collider = owner->GetComponent<SphereColliderComponent>();
    m_rigidbody = owner->GetComponent<RigidbodyComponent>();
    m_image = owner->GetComponent<ImageComponent>();

    m_switchSprite = owner->GetBehavior<SwitchSpriteBehavior>();

    m_tpsCamera = nullptr;

    m_rigidbody->SetFriction({0.95f,1.0f,0.95f});

    m_throwPowerSlider = nullptr;

    ParticleM_SetPlayer(m_transform, this);
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

    if (m_freezeTimer > 0.0f) {
        m_freezeTimer -= deltaTime;
        m_rigidbody->SetVelocity({ 0.0f,0.0f,0.0f });
        return;
    }

    // 設置確認
    if(m_groundCheckTimer > 0.0f) {
        m_groundCheckTimer -= deltaTime;
    }
    else {
        if (m_collider->GetMaxMtv().y > 0.005f) {
            m_isGrounded = true;
        }
        else {
            m_isGrounded = false;
        }
    }

    // 移動処理更新
    UpdateMovement(deltaTime);

    // ボールに乗る処理更新
    UpdateRideOnBall(deltaTime);

    // ボールを前に投げる処理更新
    UpdateThrowBall(deltaTime);

    // スコアテキスト更新
    if(m_scoreBuffer > 0.0f) {
        float buf = m_scoreBuffer * deltaTime * 5.0f;
        m_score += buf;
        m_scoreBuffer -= buf;

        // スコアテキスト更新
        std::string s = "SCORE: ";
        int value = static_cast<int>(m_score);
        for(int i = 0; i < 6; i++) {
            value /= 10;
            if(value == 0) {
                s += "0";
            }
        }
        s += std::to_string(static_cast<int>(m_score));

        std::u8string u8 = std::u8string(s.begin(), s.end());
        m_scoreText->SetText(u8);
    }
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

    // switch sprite behavior取得
    if (!m_switchSprite) {
        m_switchSprite = GetOwner()->GetBehavior<SwitchSpriteBehavior>();
    }

    // 投げるパワースライダー取得
    if (!m_throwPowerSlider) {
        GameObject* sliderObj = pScene->GetGameObjectByName("ThrowPowerSlider");
        if (sliderObj) {
            m_throwPowerSlider = sliderObj->GetComponent<SliderComponent>();
        }
    }

    // ポインターの参照取得
    if (!m_pointerTransform) {
        GameObject* pointerObj = pScene->GetGameObjectByName("Pointer");
        if (pointerObj) {
            m_pointerTransform = pointerObj->GetComponent<TransformComponent>();
        }
    }

    // スコアテキスト取得
    if (!m_scoreText) {
        GameObject* scoreTextObj = pScene->GetGameObjectByName("ScoreText");
        if (scoreTextObj) {
            m_scoreText = scoreTextObj->GetComponent<TextComponent>();
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

    if (m_ballBehavior) {
        m_ballBehavior->SetMoveDirection(moveDir);

        // ジャンプ
        if (Keyboard_IsKeyDownTrigger(KK_SPACE) && m_ballBehavior->IsGrounded() && m_groundCheckTimer <= 0.0f) {
            m_ballBehavior->AddBounceVelocity({ 0.0f,9.0f,0.0f });
            m_groundCheckTimer = 0.8f;
        }
    }
    else {
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
        if (Keyboard_IsKeyDownTrigger(KK_SPACE) && m_isGrounded) {
            velocity.y += 9.0f;
            m_groundCheckTimer = 0.8f;
        }

        // 適用処理
        m_rigidbody->SetVelocity(velocity);
    }
}

void PlayerBehavior::UpdateRideOnBall(float deltaTime)
{
    // ボールに既に乗っている場合
    if (m_ballObject) {
        // ボールの位置に合わせてプレイヤーを移動
        XMFLOAT3 ballPos = m_ballTransform->GetPosition();
        m_transform->SetPosition({
            ballPos.x,
            ballPos.y + 1.7f,
            ballPos.z
            });
        return;
    }

    // ボールに上から乗った場合
    for(int i = 0; i < ColliderComponent::MAX_COLLISION_DATA; i++) {
        auto collisionData = m_collider->GetCollisionData(i);
        if(!collisionData.GetCollisionStay()) continue;
        GameObject* otherObj = collisionData.m_other->GetOwner();
        if(otherObj->GetName() == "Ball") {

            if(collisionData.m_mtv.y <= 0.0f) {
                // 下から突き抜けた場合は無視
                continue;
            }

            // ボールに乗る処理
            m_ballObject = otherObj;
            m_ballTransform = m_ballObject->GetComponent<TransformComponent>();
            m_ballBehavior = m_ballObject->GetBehavior<BallBehavior>();
            // ボールの上にプレイヤーを配置
            XMFLOAT3 ballPos = m_ballTransform->GetPosition();
            m_transform->SetPosition({
                ballPos.x,
                ballPos.y + 1.1f,
                ballPos.z
                });

            // switch sprite が参照する速度をボールの速度に変更
            m_switchSprite->SetRigidbodyComponent(m_ballObject->GetComponent<RigidbodyComponent>());

            // プレイヤーのレイヤーを変更
            m_collider->SetLayer(ColliderComponent::Layer::PlayerOnBall);
            m_rigidbody->SetMass(0.0f); // 軽くする

            break;
        }
    }
}

void PlayerBehavior::UpdateThrowBall(float deltaTime)
{
    if (!m_ballObject) return;

    m_throwDirection = m_pointerTransform->GetPosition();
    {
        XMFLOAT3 playerPos = m_transform->GetPosition();
        m_throwDirection.x -= playerPos.x;
        m_throwDirection.y -= playerPos.y;
        m_throwDirection.z -= playerPos.z;
    }
    m_throwDirection = MiMath::Normalize(m_throwDirection);
    
    if(Keyboard_IsKeyDown(KK_F)) {
        m_tpsCamera->SetSlowMotion(true);
        m_throwPower += 30.0f * FPS_GetUnscaledDeltaTime();
        if (m_throwPower > m_throwPowerMax) {
            m_throwPower = 0;
        }
        if (m_throwPowerSlider) {
            m_throwPowerSlider->SetValue(m_throwPower / m_throwPowerMax);
        }

        return;
    }
    if(Keyboard_IsKeyUpTrigger(KK_F)){
        m_tpsCamera->SetSlowMotion(false);

        // ボールを前に飛ばす
        m_ballBehavior->AddBounceVelocity({
            m_throwDirection.x* m_throwPower,
            m_throwDirection.y* m_throwPower,
            m_throwDirection.z* m_throwPower,
            });

        // switch sprite が参照する速度をプレイヤーの速度に戻す
        m_switchSprite->SetRigidbodyComponent(m_rigidbody);

        // 切り離し処理
        m_ballObject = nullptr;
        m_ballTransform = nullptr;
        m_ballBehavior = nullptr;

        // プレイヤーのレイヤーを元に戻す
        m_collider->SetLayer(ColliderComponent::Layer::Player);
        m_rigidbody->SetMass(1.5f);

        // Freezeタイマーセット
        m_freezeTimer = 0.5f;
    }
}

void PlayerBehavior::AddScore(int score)
{
    m_scoreBuffer += score;
}
