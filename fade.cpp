//===================================================
// fade.cpp [フェードクラス]
// 
// Author：Miu Kitamura
// Date  ：2025/09/08
//===================================================
#include "fade.h"

#include "shader.h"

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

FadeObject g_Fade;
static ID3D11ShaderResourceView* g_Texture;

//===================================================
// フェード初期化
//===================================================
void Fade_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (!g_pDevice)g_pDevice = pDevice;
    if (!g_pContext)g_pContext = pContext;

    // テクスチャ画像読み込み
    LoadTexture(&g_Texture, L"asset\\Texture\\fade.bmp");
}

//===================================================
// フェード終了処理
//===================================================
void Fade_Finalize()
{
    SAFE_RELEASE(g_Texture);
}

//===================================================
// フェード更新処理
//===================================================
void Fade_Update()
{

}

//===================================================
// フェード描画処理
//===================================================
void Fade_Draw()
{
    switch (g_Fade.state) {
    case FADE_STATE::FADE_NONE:
        break;
    case FADE_STATE::FADE_IN:
        if (g_Fade.fadeColor.w < 0.0f) {
            // フェードイン終了
            g_Fade.fadeColor.w = 0.0f;
            g_Fade.state = FADE_STATE::FADE_NONE;
        }
        break;
    case FADE_STATE::FADE_OUT:
        if (g_Fade.fadeColor.w > 1.0f) {
            // フェードアウト終了
            g_Fade.fadeColor.w = 1.0f;
            SetFade(g_Fade.frame, g_Fade.fadeColor, 
                FADE_STATE::FADE_IN, g_Fade.scene);
            SetScene(g_Fade.scene);
        }
        break;
    }

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
    SetBlendState(BLENDSTATE_ALFA);

    /*DrawSprite(
        { SCREEN_WIDTH / 2.0f,SCREEN_HEIGHT / 2.0f },
        { SCREEN_WIDTH,SCREEN_HEIGHT },
        g_Fade.fadeColor);*/

    // フェード処理
    switch (g_Fade.state) {
    case FADE_STATE::FADE_IN:
        g_Fade.fadeColor.w -= (1.0f / g_Fade.frame);
        break;
    case FADE_STATE::FADE_OUT:
        g_Fade.fadeColor.w += (1.0f / g_Fade.frame);
        break;
    default:break;
    }
}

void SetFade(int fadeFrame, XMFLOAT4 color, FADE_STATE state, SCENE scene)
{
    g_Fade.frame = fadeFrame;
    g_Fade.fadeColor = color;
    g_Fade.state = state;
    g_Fade.scene = scene;

    if (g_Fade.state == FADE_IN) {
        g_Fade.fadeColor.w = 1.0f;
    }
    else if (g_Fade.state == FADE_OUT) {
        g_Fade.fadeColor.w = 0.0f;
    }
}

FADE_STATE GetFadeState()
{
    return FADE_STATE();
}
