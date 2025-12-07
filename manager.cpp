//===================================================
// manager.cpp [マネージャー]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "manager.h"

#include "title.h"
#include "game.h"

#include "fade.h"

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static SCENE g_Scene = SCENE::SCENE_NONE;

//===================================================
// スコア初期化処理
//===================================================
void Manager_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (!g_pDevice)g_pDevice = pDevice;
    if (!g_pContext)g_pContext = pContext;

    Fade_Initialize(pDevice, pContext);
    SetScene(SCENE::SCENE_GAME);
}

//===================================================
// スコア終了処理
//===================================================
void Manager_Finalize()
{
    Fade_Finalize();
    SetScene(SCENE::SCENE_NONE);
}

//===================================================
// スコア更新処理
//===================================================
void Manager_Update()
{
    switch (g_Scene) {
    case SCENE::SCENE_TITLE:
        Title_Update();
        break;
    case SCENE::SCENE_GAME:
        Game_Update();
        break;
    case SCENE::SCENE_RESULT:

        break;
    default:break;
  }
    Fade_Update();
}

//===================================================
// スコア描画処理
//===================================================
void Manager_Draw()
{
    switch (g_Scene) {
    case SCENE::SCENE_TITLE:
        Title_Draw();
        break;
    case SCENE::SCENE_GAME:
        Game_Draw();
        break;
    case SCENE::SCENE_RESULT:

        break;
    default:break;
    }
    Fade_Draw();
}

void SetScene(SCENE scene)
{
    // 現在のシーンの終了処理
    switch (g_Scene) {
    case SCENE::SCENE_TITLE:
        Title_Finalize();
        break;
    case SCENE::SCENE_GAME:
        Game_Finalize();
        break;
    case SCENE::SCENE_RESULT:

        break;
    default:break;
    }

    // シーン切り替え
    g_Scene = scene;

    // 次のシーンの初期化
    switch (g_Scene) {
    case SCENE::SCENE_TITLE:
        Title_Initialize(g_pDevice, g_pContext);
        break;
    case SCENE::SCENE_GAME:
        Game_Initialize(g_pDevice,g_pContext);
        break;
    case SCENE::SCENE_RESULT:

        break;
    default:break;
    }
}