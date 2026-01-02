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

class TpsCameraBehavior : public Behavior {
private:
    TransformComponent* m_transform;
    CameraComponent*    m_camera;

    // 注視対象のTransformComponent
    TransformComponent* m_targetTransform;

    XMFLOAT3    m_cameraAnchor; // カメラの注視点
    XMFLOAT3    m_cameraPos;    // カメラの相対位置

    float       m_distance = 10.0f; // カメラと注視点の距離
    float       m_angleX = 0.0f;   // カメラの水平角度
    float       m_angleY = 0.0f;  // カメラの垂直角度

public:
    TpsCameraBehavior(GameObject* owner);
    ~TpsCameraBehavior();

    void    Update(IScene* pScene) override;

private:

public:
    XMFLOAT3    GetCameraFoward();

};

#endif // tps_camera_behavior.h