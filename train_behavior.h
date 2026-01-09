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

    float   m_moveSpeed = 5.0f;

public:
    TrainBehavior(GameObject* owner);
    ~TrainBehavior();
    void    Update(IScene* pScene) override;
};

#endif // train_behavior.h