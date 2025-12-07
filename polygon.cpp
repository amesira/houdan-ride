/*==============================================================================

   ポリゴン描画 [polygon.cpp]
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "direct3d.h"

#include "shader.h"
#include "debug_ostream.h"
#include "sprite.h"

#include "keyboard.h"

// プロトタイプ宣言


// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

enum TextureID {
	TextureID_Number=0,
	TextureID_Man_1,
	TextureID_Man_3,

	TextureID_MAX,
};
static ID3D11ShaderResourceView* g_Texture[TextureID_MAX];

//===================================================
// ポリゴン初期化処理
//===================================================
void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Polygon_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return;
	}

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// テクスチャ読み込み
	for (int i = 0; i < TextureID_MAX; i++) {
		g_Texture[i] = nullptr;
	}
	LoadTexture(&g_Texture[TextureID_Number], L"asset\\texture\\number000.png");
	LoadTexture(&g_Texture[TextureID_Man_1], L"asset\\texture\\runningman001.png");
	LoadTexture(&g_Texture[TextureID_Man_3], L"asset\\texture\\runningman003.png");
}



//===================================================
// ポリゴン終了処理
//===================================================
void Polygon_Finalize(void)
{
	for (int i = 0; i < TextureID_MAX; i++) {
		if (g_Texture[i] == nullptr) continue;
		SAFE_RELEASE(g_Texture[i]);
	}
}

//===================================================
// ポリゴン描画処理
//===================================================
void Polygon_Draw(void)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点情報を書き込み
	const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
	const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

	// 頂点シェーダーに変換行列を設定
	Shader_SetMatrix(XMMatrixOrthographicOffCenterLH(
		0.0f, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		0.0f, 
		0.0f, 
		1.0f));

	//----------------------------------------------------
	// 描画テスト
	//----------------------------------------------------
	static float bno = 0; // ブロックの番号

	static XMFLOAT2 basePos = { SCREEN_WIDTH / 2 , SCREEN_HEIGHT / 2 + 200.0f };
	XMFLOAT2 defSize = { 200.0f,300.0f };
	static float rot = 0.0f;

	// キーボード入力チェック
	if (Keyboard_IsKeyDown(KK_LEFT)) {
		rot -= 0.1f;
	}
	if (Keyboard_IsKeyDown(KK_RIGHT)) {
		rot += 0.1f;
	}

	if (Keyboard_IsKeyDownTrigger(KK_UP)) {
		SetBlendState(BLENDSTATE_ADD);
	}
	else {
		SetBlendState(BLENDSTATE_ALFA);
	}

	g_pContext->PSSetShaderResources(0, 1, &g_Texture[TextureID_Man_1]);
	DrawSpriteExRotation({ basePos.x,basePos.y }, defSize, { 1.0f,1.0f,1.0f,1.0f }, (int)(bno * 2.0f), 5, 2, rot);

	g_pContext->PSSetShaderResources(0, 1, &g_Texture[TextureID_Man_3]);
	DrawSpriteEx({ basePos.x + 200.0f,basePos.y }, { defSize.x / 2.0f,defSize.y / 2.0f }, { 1.0f,1.0f,1.0f,1.0f }, (int)(bno * 2.0f), 5, 2);

	bno += 10.0f / 60.0f;
	if (bno > 10.0f) {
		bno = 0.0f;
	}
}
