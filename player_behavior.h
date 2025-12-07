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
class BoxColliderComponent;
class RigidbodyComponent;

class PlayerBehavior :public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    CubemeshComponent* m_cubemesh = nullptr;
    BoxColliderComponent* m_collider = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;

public:
    // scriptタイプ取得
    static Type GetTypeStatic() {
        return Type::Player;
    }
    Type GetType() const override {
        return GetTypeStatic();
    };

    PlayerBehavior(GameObject* owner);
    ~PlayerBehavior();

    void    Update()override;

private:

public:
};

#endif