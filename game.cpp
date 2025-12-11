//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "Audio.h"
#include "polygon3d.h"
#include "processor_manager.h"

#include "factory.h"

//===================================================
// ゲームシーン初期化処理
//===================================================
void GameScene::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    Polygon3D_Initialize(pDevice, pContext);

    ProcessorM_Initialize();

    GameObject* player = this->CreateGameObject();
    Factory::CreateTestPlayer(player, { -2.0f,0.5f,0.0f });

    GameObject* fieldCube = this->CreateGameObject();
    Factory::CreateBox(fieldCube, { 0.0f,-1.0f,0.0f }, { 0.2f,0.2f,0.2f,1.0f }, { 4.0f,1.0f,5.0f });
    GameObject* cube = this->CreateGameObject();
    Factory::CreateBox(cube, { 0.0f,0.0f,0.0f }, { 0.2f,1.0f,1.0f,1.0f });

    // ui
    GameObject* uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 800.0f, 500.0f, 0.0f }, u8"Hello, DirectX11!", 40.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
}

//===================================================
// ゲームシーン終了処理
//===================================================
void GameScene::Finalize()
{
    ProcessorM_Finalize();

    Polygon3D_Finalize();
}

//===================================================
// ゲームシーン更新処理
//===================================================
void GameScene::Update()
{
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        if (!obj.GetActive())continue;
        obj.Update();
    }

    ProcessorM_Update(this);
}

//===================================================
// ゲームシーン描画処理
//===================================================
void GameScene::Draw()
{
    ProcessorM_Draw(this);
}
