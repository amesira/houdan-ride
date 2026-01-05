//---------------------------------------------------
// enemy_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/05
//---------------------------------------------------
#ifndef ENEMY_BEHAVIOR_H
#define ENEMY_BEHAVIOR_H

#include "behavior.h"
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

public:
    EnemyBehavior(GameObject* owner);
    ~EnemyBehavior();
    void    Update(IScene* pScene) override;

};

#endif // ENEMY_BEHAVIOR_H