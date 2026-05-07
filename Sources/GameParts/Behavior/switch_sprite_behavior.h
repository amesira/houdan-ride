// switch_sprite_behavior.h
// - アニメーション画像の切り替えを行なう。
// - プレイヤーと敵で共通利用する。
#ifndef SWITCH_SPRITE_BEHAVIOR_H
#define SWITCH_SPRITE_BEHAVIOR_H

#include "behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class RigidbodyComponent;
class ImageComponent;

class TpsCameraBehavior;

class SwitchSpriteBehavior : public Behavior {
private:
    ImageComponent* m_image = nullptr;
    RigidbodyComponent* m_rigidbody = nullptr;

    // tpsカメラの参照
    TpsCameraBehavior* m_tpsCamera = nullptr;

    // アニメーション
    float   m_frameTimer = 0.0f;
    int     m_directIndex = 0;

public:
    SwitchSpriteBehavior(GameObject* owner);
    ~SwitchSpriteBehavior();
    void    Update(IScene* pScene) override;

    // 外部設定用
    void    SetImageComponent(ImageComponent* image) { m_image = image; }
    void    SetRigidbodyComponent(RigidbodyComponent* rigidbody) { m_rigidbody = rigidbody; }
};

#endif // SWITCH_SPRITE_BEHAVIOR_H