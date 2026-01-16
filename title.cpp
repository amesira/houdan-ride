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

#include "Audio.h"
#include "processor_manager.h"
#include "factory.h"

#include "mouse.h"

#include "image_component.h"
#include "level_manager.h"
#include "tps_camera_behavior.h"
#include "button_behavior.h"

void TitleScene::Initialize()
{
    this->Reset();

    ProcessorM_Initialize();

    // 初期化
    m_isSelectScene = false;
    m_titleUiObjects.clear();
    m_selectUiObjects.clear();

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateTpsCamera(camera, { 0.0f,3.0f,0.0f }, { 0.0f,0.0f,0.0f });
    TpsCameraBehavior* tpsCameraBe = camera->GetBehavior<TpsCameraBehavior>();
    tpsCameraBe->SetFreeze(true);

    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateLight(light, { 0.5f,-1.0f,0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.65f,0.65f,0.65f,1.0f });

    // player
    GameObject* player = this->CreateGameObject();
    Factory::CreatePlayer(player, { -2.0f,2.0f,-3.0f });

    // Title ui
    {
        GameObject* titleLogo = this->CreateGameObject();
        Factory::CreateUiImage(titleLogo, { 1280.0f / 2.0f, 80.0f, 0.0f }, 0.0f, { 400.0f,400.0f }, L"asset\\Texture\\Title.png");
        titleLogo->SetName("TitleLogo");
        m_titleUiObjects.push_back(titleLogo);

        GameObject* uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 170.0f, 0.0f }, u8"画面をクリック！", 30.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
        uiText->SetName("ClickText");
        m_titleUiObjects.push_back(uiText);
    }

    // Select ui
    {
        GameObject* uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 100.0f, 0.0f }, u8"朝の海", 70.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
        m_selectUiObjects.push_back(uiText);
        
        uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 150.0f, 0.0f }, u8"比較的穏やかな朝の海。海賊モンスターも少なめで、砂金が集めやすいぞ。", 30.0f, { 1.0f,1.0f,0.8f,1.0f }, true);
        m_selectUiObjects.push_back(uiText);

        GameObject* uiButton = this->CreateGameObject();
        Factory::CreateUiButton(uiButton, { 1280.0f / 2.0f, 600.0f }, { 300.0f,80.0f },{1.0f, 1.0f, 1.0f,1.0f});
        uiButton->SetName("StartButton_MorningSea");
        m_selectUiObjects.push_back(uiButton);
        m_startButtonMorningSeaBe = uiButton->GetBehavior<ButtonBehavior>();

        uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 610.0f, 0.0f }, u8"スタート！", 50.0f, { 0.0f,0.8f,1.0f,1.0f }, true);
        m_selectUiObjects.push_back(uiText);
    }
    for (auto& ui : m_selectUiObjects) {
        ui->SetActive(false);
    }

    LevelM_Initialize(this, true);

    m_isSelectScene = false;
}

void TitleScene::Finalize()
{
    ProcessorM_Finalize();
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Destroy();
    }

    m_titleUiObjects.clear();
    m_selectUiObjects.clear();

    LevelM_Finalize();
}

void TitleScene::Update()
{
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Update();
    }

    ProcessorM_Update(this);

    LevelM_Update(this);

    if (Mouse_IsButtonDownTrigger(Mouse_Button::LEFT)) {
        if(m_isSelectScene == false) {
            m_isSelectScene = true;

            GameObject* tpsCamera = this->GetGameObjectByName("TPSCamera");
            TpsCameraBehavior* tpsCameraBe = tpsCamera->GetBehavior<TpsCameraBehavior>();
            tpsCameraBe->SetCameraPosOffset({ 30.0f,0.0f,0.0f });

            // UI切り替え
            for(auto& ui : m_titleUiObjects) {
                ui->SetActive(false);
            }
            for(auto& ui : m_selectUiObjects) {
                ui->SetActive(true);
            }
        }

        // 
        if (m_startButtonMorningSeaBe->GetIsPressed()) {
            SetFade(60, { 0.0f,1.0f,1.0f,1.0f }, FADE_STATE::FADE_OUT, SCENE::SCENE_GAME);
        }
    }

    // 破棄予約されたGameObjectの収集
    this->CollectDestroyedGameObjects();
}

void TitleScene::Draw()
{
    ProcessorM_Draw(this);
}
