//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "field.h"
#include "Audio.h"
#include "polygon3d.h"
#include "processor_manager.h"

#include "factory.h"

// サウンド管理ID
static int g_BgmID = NULL;

static std::vector<GameObject*> g_SceneObjects = {};

//===================================================
// ゲームシーン初期化処理
//===================================================
void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    field_Initialize(pDevice, pContext);
    Polygon3D_Initialize(pDevice, pContext);

    ProcessorM_Initialize();

    // サウンド読み込み・再生
    g_BgmID = LoadAudio("asset\\Audio\\bgm.wav");
    //PlayAudio(g_BgmID, true);

    // カメラ1追加

    GameObject* player = Factory::CreateTestPlayer({ -2.0f,0.5f,0.0f });
    g_SceneObjects.push_back(player);
    GameObject* fieldCube = Factory::CreateBox({ 0.0f,-1.0f,0.0f }, { 0.2f,0.2f,0.2f,1.0f }, { 4.0f,1.0f,5.0f });
    g_SceneObjects.push_back(fieldCube);
    GameObject* cube = Factory::CreateBox({ 0.0f,0.0f,0.0f }, { 0.2f,1.0f,1.0f,1.0f });
    g_SceneObjects.push_back(cube);

    // ui
    GameObject* uiText = Factory::CreateUiText({ 800.0f, 500.0f, 0.0f }, u8"Hello, DirectX11!", 40.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
    g_SceneObjects.push_back(uiText);
}

//===================================================
// ゲームシーン終了処理
//===================================================
void Game_Finalize()
{
    ProcessorM_Finalize();

    Polygon3D_Finalize();
    field_Finalize();

    UnloadAudio(g_BgmID);
}

//===================================================
// ゲームシーン更新処理
//===================================================
void Game_Update()
{
    for (GameObject* gameObject : g_SceneObjects) {
        gameObject->Update();
    }
    ProcessorM_Update();
}

//===================================================
// ゲームシーン描画処理
//===================================================
void Game_Draw()
{
    //field_Draw();

    ProcessorM_Draw();
}
