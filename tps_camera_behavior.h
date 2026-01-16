//---------------------------------------------------
// tps_camera_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2025/12/31
//---------------------------------------------------
#ifndef TPS_CAMERA_BEHAVIOR_H
#define TPS_CAMERA_BEHAVIOR_H

#include "behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class CameraComponent;
class ImageComponent;

class TpsCameraBehavior : public Behavior {
private:
    TransformComponent* m_transform;
    CameraComponent*    m_camera;

    // 注視対象のTransformComponent
    TransformComponent* m_targetTransform;

    XMFLOAT3    m_cameraAnchor; // カメラの注視点
    XMFLOAT3    m_cameraPos;    // カメラの相対位置

    float       m_distance = 15.0f; // カメラと注視点の距離
    float       m_angleX = 0.0f;   // カメラの水平角度
    float       m_angleY = 0.0f;  // カメラの垂直角度

    // 投影面に映し出すImageComponent
    ImageComponent* m_cameraImageComp;

    bool    m_isSlowMotion = false;

    XMFLOAT3 m_cameraAtOffset = { 0.0f,0.0f,0.0f };
    XMFLOAT3 m_cameraPosOffset = {0.0f,0.0f,0.0f};
    bool    m_isFreeze = false;

    bool    m_useSlowMotion = false;

public:
    TpsCameraBehavior(GameObject* owner);
    ~TpsCameraBehavior();

    void    Update(IScene* pScene) override;

private:

public:
    XMFLOAT3    GetCameraFoward();
    void    SetSlowMotion(bool isSlow) { m_isSlowMotion = isSlow; }

    void    SetFreeze(bool isFreeze) { m_isFreeze = isFreeze; }

    void    SetCameraAtOffset(XMFLOAT3 offset) { m_cameraAtOffset = offset; }
    void    SetCameraPosOffset(XMFLOAT3 offset) { m_cameraPosOffset = offset; }

    void    UseSlowMotion(bool useSlow) { m_useSlowMotion = useSlow; }
};

#endif // tps_camera_behavior.h