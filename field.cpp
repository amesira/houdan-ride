//===================================================
// field.cpp [フィールド制御]
// 
// Author：Miu Kitamura
// Date  ：2025/06/16
//===================================================
#include "field.h"

#include "sprite.h"
#include "shader.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11ShaderResourceView* g_Texture;

void field_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// テクスチャ画像読み込み
	LoadTexture(&g_Texture, L"asset\\Texture\\fade.bmp");
}

void field_Finalize(void)
{
	SAFE_RELEASE(g_Texture);
}

void field_Draw(void)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 画面サイズ取得
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	//----------------------------------------------------
	// 描画前の設定処理
	//----------------------------------------------------
	// 頂点シェーダーに変換行列を設定
	Shader_SetMatrix(XMMatrixOrthographicOffCenterLH(
		0.0f,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		0.0f,
		0.0f,
		1.0f));

	// テクスチャ使用設定
	g_pContext->PSSetShaderResources(0, 1, &g_Texture);

	//----------------------------------------------------
	// 画面サイズのスプライトを描画
	//----------------------------------------------------
	SetBlendState(BLENDSTATE_NONE);
	
	DrawSprite(
		{ SCREEN_WIDTH / 2.0f,SCREEN_HEIGHT / 2.0f },
		{ SCREEN_WIDTH,SCREEN_HEIGHT },
		{ 0.2f,0.5f,0.5f,1.0f });
}