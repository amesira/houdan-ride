//===================================================
// sprite.h [スプライト]
// 
// Author：Miu Kitamura
// Date  ：2025/06/17
//===================================================
#pragma once

//----------------------------------------------------
// インクルード
//----------------------------------------------------
#include <d3d11.h>
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;

//----------------------------------------------------
// 構造体
//----------------------------------------------------

/// @brief 頂点構造体
struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;		// 頂点カラー（R,G,B,A）
	XMFLOAT2 texCoord;	// テクスチャ座標
};

//----------------------------------------------------
// スプライト 初期化・終了処理
//----------------------------------------------------

/// @brief スプライトを初期化する
void InitializeSprite();

void LoadTexture(ID3D11ShaderResourceView** texture, const wchar_t* fileName);

/// @brief スプライトを終了・頂点バッファの解放
void FinalizeSprite();

//----------------------------------------------------
// スプライト描画処理
//----------------------------------------------------

/// @brief スプライト描画関数
void DrawSprite(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color = {1.0f,1.0f,1.0f,1.0f});

/// @brief スプライト描画関数EX：スプライト分割
/// @param bno 分割番号
/// @param wc 分割数（横）
/// @param hc 分割数（縦）
void DrawSpriteEx(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color,
    int bno, int wc, int hc);

/// @brief スクロール用スプライト描画
void DrawSpriteScroll(XMFLOAT2 pos, XMFLOAT2 size, 
	XMFLOAT4 color,XMFLOAT2 texcoord);

/// @brief スプライト描画EX：スプライト分割 + 回転
void DrawSpriteExRotation(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color,
	int bno, int wc, int hc,float rad);

/// @brief スプライト描画（行列使用ver）
void DrawSprite(XMFLOAT2 size,XMFLOAT4 color,int bno, int wc,int hc);