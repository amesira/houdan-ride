#pragma once
#include "Sources/Core/behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class BoxColliderComponent;

class GolfBehavior : public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    BoxColliderComponent* m_collider = nullptr;

    GameObject* m_ballObject = nullptr;
    bool m_isCleared = false;

public:
    GolfBehavior(GameObject* owner);
    ~GolfBehavior();
    void    Update(IScene* pScene) override;

};