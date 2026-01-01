//----------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H

#include "behavior.h"

class TransformComponent;
class CubemeshComponent;
class ColliderComponent;
class RigidbodyComponent;

class TpsCameraBehavior;

class PlayerBehavior :public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    CubemeshComponent* m_cubemesh = nullptr;
    ColliderComponent* m_collider = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;

    // TPSカメラの参照
    TpsCameraBehavior* m_tpsCamera = nullptr;

public:
    PlayerBehavior(GameObject* owner);
    ~PlayerBehavior();

    void    Update(IScene* pScene)override;

private:

public:
};

#endif