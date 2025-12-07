//----------------------------------------------------
// game.h [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//----------------------------------------------------
#ifndef GAME_H
#define GAME_H

// directx
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

// メイン処理
void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Game_Finalize();
void Game_Update();
void Game_Draw();

#endif