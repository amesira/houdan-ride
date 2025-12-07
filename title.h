//----------------------------------------------------
// title.h [タイトルシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//----------------------------------------------------
#ifndef TITLE_H
#define TITLE_H

// directx
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

// メイン処理
void Title_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Title_Finalize();
void Title_Update();
void Title_Draw();

#endif