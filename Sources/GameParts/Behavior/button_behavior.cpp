#include "button_behavior.h"
#include "Sources/Scene/scene_interface.h"
#include "Sources/Core/game_object.h"
#include "Sources/Core/type_id.h"

#include "Sources/GameParts/Component/UiComponents/rect_transform_component.h"
#include "Sources/GameParts/Component/UiComponents/image_component.h"

#include "Sources/System/Device/mouse.h"

ButtonBehavior::ButtonBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<ButtonBehavior>())
{
    m_rectTransform = owner->GetComponent<RectTransformComponent>();
    m_imageComponent = owner->GetComponent<ImageComponent>();
}

ButtonBehavior::~ButtonBehavior()
{

}

void ButtonBehavior::Update(IScene* pScene)
{
    // マウスのスクリーン座標取得
    XMFLOAT2 mousePos = { Mouse_GetPositionX(), Mouse_GetPositionY() };

    // ボタンの位置とサイズを取得
    XMFLOAT2 buttonPos = { m_rectTransform->GetPosition().x, m_rectTransform->GetPosition().y };
    XMFLOAT2 buttonSize = { m_rectTransform->GetScaling().x, m_rectTransform->GetScaling().y };

    // ボタンの範囲内にカーソルがあるかどうか
    m_isHover = (
        mousePos.x >= buttonPos.x - buttonSize.x / 2.0f &&
        mousePos.x <= buttonPos.x + buttonSize.x / 2.0f &&
        mousePos.y >= buttonPos.y - buttonSize.y / 2.0f &&
        mousePos.y <= buttonPos.y + buttonSize.y / 2.0f);

    // pressed状態の更新
    m_isPressed = false;
    if (m_isHover) {
        if (Mouse_IsButtonDownTrigger(Mouse_Button::LEFT)) {
            m_isPressed = true;
        }
    }

    // 画像の色を変更
    if (m_isPressed) {
        XMFLOAT4 pressedColor = { m_baseColor.x * 0.6f, m_baseColor.y * 0.6f, m_baseColor.z * 0.6f, m_baseColor.w };
        m_imageComponent->SetColor(pressedColor);
    }
    else if (m_isHover) {
        XMFLOAT4 hoverColor = { m_baseColor.x * 0.8f, m_baseColor.y * 0.8f, m_baseColor.z * 0.8f, m_baseColor.w };
        m_imageComponent->SetColor(hoverColor);
    }
    else {
        m_imageComponent->SetColor(m_baseColor);
    }
}
