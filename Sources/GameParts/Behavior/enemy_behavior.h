//---------------------------------------------------
// enemy_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//---------------------------------------------------
#ifndef ENEMY_BEHAVIOR_H
#define ENEMY_BEHAVIOR_H

#include "Sources/Core/behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class RigidbodyComponent;
class SphereColliderComponent;
class ImageComponent;

class EnemyBehavior : public Behavior {
private:
    TransformComponent*          m_transform = nullptr;
    RigidbodyComponent*          m_rigidbody = nullptr;
    SphereColliderComponent*     m_collider = nullptr;
    ImageComponent*              m_image = nullptr;

    TransformComponent* m_playerTransform = nullptr;

    XMFLOAT3 m_force = { 0.0f, 0.0f, 0.0f };

public:
    EnemyBehavior(GameObject* owner);
    ~EnemyBehavior();
    void    Update(IScene* pScene) override;

    void    AddForce(XMFLOAT3 force) {
        m_force.x += force.x;
        m_force.y += force.y;
        m_force.z += force.z;
    }
};

#endif // ENEMY_BEHAVIOR_H