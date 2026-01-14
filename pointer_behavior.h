// マウスカーソルを表現するビヘイビア
#pragma once
#include "behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class ImageComponent;
class CameraComponent;
class RectTransformComponent;

class TpsCameraBehavior;

class PointerBehavior : public Behavior {
private:
    TransformComponent* m_transform;
    ImageComponent*     m_imageComp;
    float   m_moveSpeed = 10.0f;

    TransformComponent* m_cameraTransform;
    CameraComponent* m_cameraComp;

    RectTransformComponent* m_sliderRectTransform;

    float m_distanceFromCamera = 40.0f;

public:
    PointerBehavior(GameObject* owner);
    ~PointerBehavior();
    void    Update(IScene* pScene) override;

};