//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "Audio.h"
#include "processor_manager.h"

#include "factory.h"
#include "particle_manager.h"

#include "level_manager.h"

//===================================================
// ゲームシーン初期化処理
//===================================================
void GameScene::Initialize()
{
    ProcessorM_Initialize();

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateTpsCamera(camera, { 0.0f,3.0f,0.0f }, { 0.0f,5.0f,0.0f });
    
    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateLight(light, { 0.5f,-1.0f,0.5f,0.0f }, { 0.1f,0.1f,0.1f,1.0f }, { 0.7f,0.7f,0.7f,1.0f });

    GameObject* player = this->CreateGameObject();
    Factory::CreatePlayer(player, { -2.0f,2.0f,-3.0f });

    // ui
    GameObject* uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 10.0f, 50.0f, 0.0f }, u8"SCORE: 0000000", 50.0f, { 1.0f,1.0f,1.0f,1.0f },false);
    uiText->SetName("ScoreText");

    uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 350.0f, 50.0f, 0.0f }, u8"-500", 30.0f, { 1.0f,0.0f,0.0f,1.0f }, false);
    uiText->SetName("PenaltyText");

    /*GameObject* uiImage = this->CreateGameObject();
    Factory::CreateUiImage(uiImage, { 400.0f, 50.0f, 0.0f }, 0.0f, { 200.0f,200.0f }, L"asset\\Texture\\map.png");
    */
    GameObject* uiSlider = this->CreateGameObject();
    Factory::CreateUiSlider(uiSlider, { 400.0f, 500.0f, 0.0f }, 90.0f, { 100.0f,30.0f });
    uiSlider->SetName("ThrowPowerSlider");



    // pointer
    GameObject* pointer = this->CreateGameObject();
    Factory::CreatePointer(pointer);

    LevelM_Initialize(this);
}

//===================================================
// ゲームシーン終了処理
//===================================================
void GameScene::Finalize()
{
    ProcessorM_Finalize();
}

//===================================================
// ゲームシーン更新処理
//===================================================
void GameScene::Update()
{
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Update();
    }

    ProcessorM_Update(this);

    LevelM_Update(this);

    // 破棄予約されたGameObjectの収集
    this->CollectDestroyedGameObjects();
}

//===================================================
// ゲームシーン描画処理
//===================================================
void GameScene::Draw()
{
    ProcessorM_Draw(this);
}
