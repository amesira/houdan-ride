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

    float m_freezeTimer = 0.0f;

    bool m_isGrounded = false;
    float m_groundCheckTimer = 0.0f;

    // TPSカメラの参照
    TpsCameraBehavior* m_tpsCamera = nullptr;

    // ボールの参照
    GameObject*         m_ballObject = nullptr;
    TransformComponent* m_ballTransform = nullptr;
    BallBehavior*       m_ballBehavior = nullptr;

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

};

#endif