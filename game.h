//----------------------------------------------------
// game_scene.h [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//----------------------------------------------------
#ifndef GAME_H
#define GAME_H

#include "scene_base.h"

// ゲームシーン
class GameScene : public SceneBase {
public:
    void    Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
    void    Finalize() override;

    void    Update() override;
    void    Draw() override;
};

#endif