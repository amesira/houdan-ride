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

class PlayerBehavior :public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    ColliderComponent* m_collider = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;

    // charaの参照
    TransformComponent* m_charaTransform = nullptr;
    ImageComponent*     m_charaImage = nullptr;
    float               m_charaAnimTimer = 0.0f;

    // TPSカメラの参照
    TpsCameraBehavior* m_tpsCamera = nullptr;

    // 回転
    XMVECTOR m_quaternion = XMQuaternionIdentity();

public:
    PlayerBehavior(GameObject* owner);
    ~PlayerBehavior();

    void    Update(IScene* pScene)override;

private:

public:
};

#endif