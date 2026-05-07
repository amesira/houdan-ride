//===================================================
// manager.cpp [マネージャー]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "manager.h"

#include "Sources/Scene/title.h"
#include "Sources/Scene/game.h"
#include "Sources/Scene/result.h"

#include "Sources/Content/fade.h"

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static IScene* g_SceneInstance[SCENE::SCENE_MAX];
static SCENE g_Scene = SCENE::SCENE_NONE;

static GameScene* g_pGameScene = nullptr;
static ResultScene* g_pResultScene = nullptr;

//===================================================
// スコア初期化処理
//===================================================
void Manager_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (!g_pDevice)g_pDevice = pDevice;
    if (!g_pContext)g_pContext = pContext;

    g_SceneInstance[SCENE::SCENE_NONE] = nullptr;
    g_SceneInstance[SCENE::SCENE_TITLE] = new TitleScene();

    g_pGameScene = new GameScene();
    g_SceneInstance[SCENE::SCENE_GAME] = g_pGameScene;

    g_pResultScene = new ResultScene();
    g_SceneInstance[SCENE::SCENE_RESULT] = g_pResultScene;

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
    g_SceneInstance[g_Scene]->Update();
    Fade_Update();
}

//===================================================
// スコア描画処理
//===================================================
void Manager_Draw()
{
    g_SceneInstance[g_Scene]->Draw();
    Fade_Draw();


    Direct3D_Present();
}

void Manager_SetGameLevel(int level)
{
    g_pGameScene->SetGameLevel(level);
    g_pResultScene->SetLevelID(level);
}

void Manager_SendScore(int score)
{
    g_pResultScene->SetScore(score);
}

void SetScene(SCENE scene)
{
    // 現在のシーンの終了処理
    if (g_Scene != SCENE::SCENE_NONE){
        g_SceneInstance[g_Scene]->Finalize();
    }

    // シーン切り替え
    g_Scene = scene;

    // 次のシーンの初期化
    if (g_SceneInstance[g_Scene] == nullptr)return;
    g_SceneInstance[g_Scene]->Initialize();
}