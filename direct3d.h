//===================================================
// direct3d.h [Direct3Dの初期化関連]
// 
// Author：Miu Kitamura
// Date  ：2025/05/11
//===================================================
#ifndef DIRECT3D_H
#define DIRECT3D_H

#include <Windows.h>
#include <d3d11.h>
#include <mmsystem.h>

#include <DirectXMath.h>

#include "DirectXTex.h"
#if _DEBUG
 #pragma comment(lib,"DirectXTex_Debug.lib")
#else
 #pragma comment(lib,"DirectXTex_Release.lib")
#endif

/* セーフリリースマクロ */
#define SAFE_RELEASE(o) if(o){(o)->Release();o=NULL;}

bool Direct3D_Initialize(HWND hWnd);    // DirectXの初期化
void Direct3D_Finalize();               // DirectXの終了処理

void Direct3D_BeginScene();

void Direct3D_Clear();  // バックバッファのクリア
void Direct3D_Present();// バックバッファの表示

void Direct3D_ResetViewport(); // ビューポートをリセット

/* ゲッター */
ID3D11Device* Direct3D_GetDevice();
ID3D11DeviceContext* Direct3D_GetDeviceContext();

unsigned int Direct3D_GetBackBufferWidth();
unsigned int Direct3D_GetBackBufferHeight();

/* ブレンドステート関連 */
enum BLENDSTATE {
    BLENDSTATE_NONE = 0,    // ブレンドしない
    BLENDSTATE_ALFA,        // 普通のαブレンド
    BLENDSTATE_ADD,         // 加算合成
    BLENDSTATE_SUB,         // 減算合成

    BLENDSTATE_MAX,
};

void SetBlendState(BLENDSTATE blend);
void SetDepthState(bool flag);

void Direct3D_CreateSnapshotSceneSRV(ID3D11ShaderResourceView** ppSrv);

DirectX::XMMATRIX& Direct3D_GetViewMatrix();
DirectX::XMMATRIX& Direct3D_GetProjectionMatrix();
void Direct3D_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Direct3D_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

// バックバッファにSRVのテクスチャをまるごとコピーする関数
void Direct3D_CopySRVToBackBuffer(ID3D11ShaderResourceView* pSrv);

// ブロック縦横配列サイズ
#define BLOCK_COLS (6)  // ブロックスタックの横幅
#define BLOCK_ROWS (13) // ブロックスタックの縦幅

// ブロックサイズ
#define BLOCK_WIDTH  (50.0f)
#define BLOCK_HEIGHT (50.0f)


#define POSITION_OFFSET_X (490.0f)
#define POSITION_OFFSET_Y (34.0f)

#endif