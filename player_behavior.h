//----------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H

#include "behavior.h"

#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;
class ImageComponent;
class SliderComponent;
class TextComponent;

class TpsCameraBehavior;
class BallBehavior;
class SwitchSpriteBehavior;

class PlayerBehavior :public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    ColliderComponent* m_collider = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;
    ImageComponent* m_image = nullptr;

    SwitchSpriteBehavior* m_switchSprite = nullptr;

    // 凍結タイマー
    float m_freezeTimer = 0.0f;

    // 接地フラグ・タイマー
    bool m_isGrounded = false;
    float m_groundCheckTimer = 0.0f;

    // ボール投げパラメータ
    SliderComponent* m_throwPowerSlider = nullptr;
    XMFLOAT3 m_throwDirection = { 0.0f,0.0f,0.0f };
    float m_throwPower = 20.0f;
    float m_throwPowerMax = 50.0f;

    // TPSカメラの参照
    TpsCameraBehavior* m_tpsCamera = nullptr;

    // ボールの参照
    GameObject*         m_ballObject = nullptr;
    TransformComponent* m_ballTransform = nullptr;
    BallBehavior*       m_ballBehavior = nullptr;

    // ポインターの参照
    TransformComponent* m_pointerTransform = nullptr;

    // スコア
    TextComponent* m_scoreText = nullptr;
    float m_score = 0;
    float m_scoreBuffer = 0;

public:
    PlayerBehavior(GameObject* owner);
    ~PlayerBehavior();

    void    Update(IScene* pScene)override;

private:
    // 参考オブジェクトの取得
    void    GetReferenceObjects(IScene* pScene);

    // 移動処理の更新
    void    UpdateMovement(float deltaTime);

    // ボールに乗る処理の更新
    void    UpdateRideOnBall(float deltaTime);
    // ボールを前に飛ばす処理の更新
    void    UpdateThrowBall(float deltaTime);

public:
    // スコア取得・加算
    void    AddScore(int score);
    int     GetScore() const { return m_score + m_scoreBuffer;}

};

#endif