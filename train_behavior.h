//---------------------------------------------------
// train_behavior.h
// -動く列車の挙動。上に乗っている物体も一緒に動く。
// 
// Author：Miu Kitamura
// Date  ：2026/01/09
//---------------------------------------------------
#ifndef TRAIN_BEHAVIOR_H
#define TRAIN_BEHAVIOR_H

#include "behavior.h"
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

class TransformComponent;
class RigidbodyComponent;
class BoxColliderComponent;
class ModelComponent;

class TrainBehavior : public Behavior {
private:
    TransformComponent* m_transform;
    BoxColliderComponent*  m_collider;
    ModelComponent* m_model;

    float   m_moveSpeed = 0.0f;

    std::vector<TransformComponent*> m_childTransforms;
    std::vector<BoxColliderComponent*> m_childColliders;

    int m_rideCounter = 0;
    TransformComponent* m_rideTransform[32];
    int m_rideTimer[32];

    float m_timer = 0;

public:
    TrainBehavior(GameObject* owner);
    ~TrainBehavior();
    void    Update(IScene* pScene) override;

private:
    void    EntryRideObjects(BoxColliderComponent* col);

public:
    void    SetMoveSpeed(float speed) { m_moveSpeed = speed; }
    void    AddChildCollider(GameObject* collider);

    XMFLOAT3   GetPosition() const;
};

#endif // train_behavior.h