#include "pointer_behavior.h"
#include "Sources/Scene/scene_interface.h"
#include "Sources/Core/game_object.h"
#include "Sources/Core/type_id.h"

#include "Sources/System/mi_fps.h"
#include "Sources/System/Device/mouse.h"

#include "Utility/mi_math.h"
#include "Utility/debug_ostream.h"

#include "Sources/GameParts/Component/transform_component.h"
#include "Sources/GameParts/Component/UiComponents/image_component.h"
#include "Sources/GameParts/Component/camera_component.h"
#include "Sources/GameParts/Component/UiComponents/rect_transform_component.h"

PointerBehavior::PointerBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<PointerBehavior>())
{
    m_transform = owner->GetComponent<TransformComponent>();
    m_imageComp = owner->GetComponent<ImageComponent>();

    m_cameraComp = nullptr;
    m_cameraTransform = nullptr;
    m_sliderRectTransform = nullptr;
}

PointerBehavior::~PointerBehavior()
{

}

void PointerBehavior::Update(IScene* pScene)
{
    if (m_cameraComp == nullptr) {
        GameObject* cameraObj = pScene->GetGameObjectByName("TPSCamera");
        if (cameraObj) {
            m_cameraComp = cameraObj->GetComponent<CameraComponent>();
            m_cameraTransform = cameraObj->GetComponent<TransformComponent>();
        }
        return;
    }

    if(m_sliderRectTransform == nullptr) {
        GameObject* sliderObj = pScene->GetGameObjectByName("ThrowPowerSlider");
        if (sliderObj) {
            m_sliderRectTransform = sliderObj->GetComponent<RectTransformComponent>();
        }
    }

    float deltaTime = FPS_GetDeltaTime();

    // マウスのスクリーン座標取得
    XMFLOAT2 mousePos = {Mouse_GetPositionX(), Mouse_GetPositionY()};

    // mouseX, mouseY : スクリーン座標（ピクセル）
    float ndcX = (mousePos.x / Direct3D_GetBackBufferWidth()) * 2.0f - 1.0f;
    float ndcY = 1.0f - (mousePos.y / Direct3D_GetBackBufferHeight()) * 2.0f;

    //hal::dout << "mousePos: " << mousePos.x << ", " << mousePos.y << std::endl;

    XMVECTOR pNear = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
    XMVECTOR pFar = XMVectorSet(ndcX, ndcY, 1.0f, 1.0f);

    XMMATRIX invProj = XMMatrixInverse(nullptr, m_cameraComp->GetProjectionMatrix());
    XMMATRIX invView = XMMatrixInverse(nullptr, m_cameraComp->GetViewMatrix());

    XMVECTOR vNear = XMVector3TransformCoord(pNear, invProj);
    XMVECTOR vFar = XMVector3TransformCoord(pFar, invProj);

    XMVECTOR wNear = XMVector3TransformCoord(vNear, invView);
    XMVECTOR wFar = XMVector3TransformCoord(vFar, invView);

    XMVECTOR rayOrigin = wNear;
    XMVECTOR rayDir = XMVector3Normalize(wFar - wNear);

    // 位置決定
    XMVECTOR targetPos = rayOrigin + rayDir * m_distanceFromCamera; // 適当な距離（10.0f）先
    XMFLOAT3 pos;

    XMStoreFloat3(&pos, targetPos);
    
    // スムーズに移動
    m_transform->SetPosition(pos);

    // スライダー位置調整
    if(m_sliderRectTransform) {
        XMFLOAT3 sliderPos;
        sliderPos.x = mousePos.x;
        sliderPos.y = mousePos.y + 40.0f;
        m_sliderRectTransform->SetPosition(sliderPos);
    }

    // マウスホイール
   /* int wheelDelta = Mouse_GetScrollWheelValue();
    m_distanceFromCamera += static_cast<float>(wheelDelta) * 0.01f;*/
}