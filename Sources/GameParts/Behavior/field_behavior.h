#pragma once
#include "behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;

class FieldBehavior : public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    TransformComponent* m_shipTransform = nullptr;

public:
    FieldBehavior(GameObject* owner);
    ~FieldBehavior();
    void    Update(IScene* pScene) override;
};