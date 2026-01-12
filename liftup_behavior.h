#pragma once
#include "behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;

class LiftupBehavior : public Behavior {
private:
    TransformComponent* m_transform;
    XMFLOAT3 targetPos;

public:
    LiftupBehavior(GameObject* owner);
    ~LiftupBehavior();
    void    Update(IScene* pScene) override;
};