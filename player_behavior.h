//----------------------------------------------------
// player_behavior.h
// 
// Author：Miu Kitamura
// Date  ：2025/10/27
//----------------------------------------------------
#ifndef PLAYER_BEHAVIOR_H
#define PLAYER_BEHAVIOR_H

#include "behavior.h"

#include <DirectXMath.h>
using namespace DirectX;

class TransformComponent;
class ColliderComponent;
class RigidbodyComponent;
class ImageComponent;

class TpsCameraBehavior;

class PlayerBehavior :public Behavior {
private:
    TransformComponent* m_transform = nullptr;
    ColliderComponent* m_collider = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;
    ImageComponent* m_image = nullptr;

    // TPSカメラの参照
    TpsCameraBehavior* m_tpsCamera = nullptr;

public:
    PlayerBehavior(GameObject* owner);
    ~PlayerBehavior();

    void    Update(IScene* pScene)override;

private:
    // 参考オブジェクトの取得
    void    GetReferenceObjects(IScene* pScene);

    // 移動処理の更新
    void    UpdateMovement(float deltaTime);

public:

};

#endif