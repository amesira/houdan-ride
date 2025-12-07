//===================================================
// title.cpp [タイトルシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "title.h"

// graphics
#include "shader.h"
#include "sprite.h"

// system
#include "keyboard.h"

#include "manager.h"
#include "fade.h"

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11ShaderResourceView* g_Texture;

//===================================================
// タイトルシーン初期化処理
//===================================================
void Title_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// テクスチャ画像読み込み
	LoadTexture(&g_Texture, L"asset\\Texture\\Title.png");
}

//===================================================
// タイトルシーン終了処理
//===================================================
void Title_Finalize()
{
	SAFE_RELEASE(g_Texture);
}

//===================================================
// タイトルシーン更新処理
//===================================================
void Title_Update()
{
	if (Keyboard_IsKeyDownTrigger(KK_SPACE)) {
		SetFade(
			20.0f, 
			DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			FADE_STATE::FADE_OUT, 
			SCENE::SCENE_GAME);
	}
}

//===================================================
// タイトルシーン描画処理
//===================================================
void Title_Draw()
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
		{ 1.0f,1.0f,1.0f,1.0f });
}
