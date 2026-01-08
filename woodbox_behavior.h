//---------------------------------------------------
// woodbox_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2026/01/08
//---------------------------------------------------
#ifndef WOODBOX_BEHAVIOR_H
#define WOODBOX_BEHAVIOR_H

#include "behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class RigidbodyComponent;
class BoxColliderComponent;
class ModelComponent;

class WoodboxBehavior : public Behavior {
private:
    TransformComponent* m_transform;
    RigidbodyComponent* m_rigidbody;
    BoxColliderComponent*  m_collider;
    ModelComponent* m_model;

    bool   m_isBroken = false;
    
public:
    WoodboxBehavior(GameObject* owner);
    ~WoodboxBehavior();

    void    Update(IScene* pScene) override;

    void    Break(XMFLOAT3 hitPoint);
};

#endif // woodbox_behavior.h