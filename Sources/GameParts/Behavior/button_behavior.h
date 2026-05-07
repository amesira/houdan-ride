#pragma once
#include "Sources/Core/behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class RectTransformComponent;
class ImageComponent;

class ButtonBehavior : public Behavior {
private:
    RectTransformComponent* m_rectTransform = nullptr;
    ImageComponent* m_imageComponent = nullptr;

    bool m_isHover = false;
    bool m_isPressed = false;

    XMFLOAT4 m_baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
    ButtonBehavior(GameObject* owner);
    ~ButtonBehavior();
    void    Update(IScene* pScene) override;

    void    SetBaseColor(const XMFLOAT4& color) { m_baseColor = color; }

    bool    GetIsHover() const { return m_isHover; }
    bool    GetIsPressed() const { return m_isPressed; }
};