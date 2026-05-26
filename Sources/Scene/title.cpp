//===================================================
// title.cpp [タイトルシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "title.h"

// graphics
#include "Sources/System/Graphic/shader.h"
#include "Sources/System/Graphic/sprite.h"

// system
#include "Sources/System/Device/keyboard.h"

#include "Sources/Manager/manager.h"
#include "Sources/Content/fade.h"

#include "Sources/System/Device/Audio.h"
#include "Sources/Manager/processor_manager.h"
#include "Sources/GameParts/factory.h"

#include "Sources/System/Device/mouse.h"

#include "Sources/GameParts/Component/UiComponents/image_component.h"
#include "Sources/Manager/level_manager.h"
#include "Sources/GameParts/Behavior/tps_camera_behavior.h"
#include "Sources/GameParts/Behavior/ball_behavior.h"
#include "Sources/GameParts/Behavior/train_behavior.h"
#include "Sources/GameParts/Behavior/button_behavior.h"
#include "Sources/GameParts/Component/UiComponents/text_component.h"
#include "Sources/GameParts/Component/camera_component.h"
#include "Sources/GameParts/Component/transform_component.h"

#include "Sources/System/mi_fps.h"

void TitleScene::Initialize()
{
    this->Reset();

    ProcessorM_Initialize();

    FPS_SetTimeScale(1.0f);

    // 初期化
    m_isSelectScene = false;
    m_titleUiObjects.clear();
    m_selectUiObjects.clear();
    m_levelManager = nullptr;

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateTpsCamera(camera, { 0.0f,3.0f,0.0f }, { 0.0f,0.0f,0.0f });
    TpsCameraBehavior* tpsCameraBe = camera->GetBehavior<TpsCameraBehavior>();
    tpsCameraBe->SetFreeze(true);
    m_cameraComp = camera->GetComponent<CameraComponent>();

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
        m_levelTextComp = uiText->GetComponent<TextComponent>();
        
        uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 150.0f, 0.0f }, u8"比較的穏やかな朝の海。海賊モンスターも少なめで、砂金が集めやすいぞ。", 30.0f, { 1.0f,1.0f,0.8f,1.0f }, true);
        m_selectUiObjects.push_back(uiText);
        m_levelDescTextComp = uiText->GetComponent<TextComponent>();

        GameObject* uiButton = this->CreateGameObject();
        Factory::CreateUiButton(uiButton, { 1280.0f / 2.0f, 600.0f }, { 300.0f,80.0f },{1.0f, 1.0f, 1.0f,1.0f});
        uiButton->SetName("StartButton_MorningSea");
        m_selectUiObjects.push_back(uiButton);
        m_startButtonMorningSeaBe = uiButton->GetBehavior<ButtonBehavior>();

        uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 610.0f, 0.0f }, u8"スタート！", 50.0f, { 1.0f,0.8f,0.0f,1.0f }, true);
        m_selectUiObjects.push_back(uiText);

        uiButton = this->CreateGameObject();
        Factory::CreateUiButton(uiButton, { 1280.0f / 2.0f - 500.0f, 400.0f }, { 80.0f,100.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"asset\\Texture\\left.png");
        m_leftButtonBe = uiButton->GetBehavior<ButtonBehavior>();
        m_selectUiObjects.push_back(uiButton);

        uiText = this->CreateGameObject();
        Factory::CreateUiButton(uiText, { 1280.0f / 2.0f + 500.0f, 400.0f }, { 80.0f,100.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"asset\\Texture\\right.png");
        m_rightButtonBe = uiText->GetBehavior<ButtonBehavior>();
        m_selectUiObjects.push_back(uiText);
    }
    for (auto& ui : m_selectUiObjects) {
        ui->SetActive(false);
    }

    GameObject* water = this->CreateGameObject();
    water->SetName("Water");
    Factory::CreateUiImageWorld(water, XMFLOAT3(0.0f, -3.0f, 0.0f), XMFLOAT3(XMConvertToRadians(-90.0f), 0.0f, 0.0f), XMFLOAT3(100.0f, 200.0f, 1.0f));
    ImageComponent* imageComp = water->GetComponent<ImageComponent>();
    imageComp->Load(L"asset\\Texture\\water.png");
    imageComp->SetColor(XMFLOAT4(0.2f, 1.0f, 1.0f, 0.7f));

    TrainBehavior* ship = LevelObjects::CreateMainShip(this, XMFLOAT3(0.0f, -5.0f, -2.0f));

    GameObject* ball = this->CreateGameObject();
    Factory::CreateBall(ball, { 0.0f,5.0f,0.0f });

    GameObject* levelManagerObj = this->CreateGameObject();
    levelManagerObj->SetName("LevelManager");
    m_levelManager = levelManagerObj->AddBehavior<LevelManagerBehavior>();
    m_levelManager->SetTitleMode(true);
    m_levelManager->SetWater(water->GetComponent<TransformComponent>());
    m_levelManager->SetMainShip(ship);
    m_levelManager->SetMainBall(ball->GetBehavior<BallBehavior>());

    m_selectLevel = 1;
    m_levelManager->SetLevelID(m_selectLevel);
    m_levelManager->ResetProgress();
    m_isSelectScene = false;

    m_bgmHandle = LoadAudio("asset\\Audio\\title.wav");
    PlayAudio(m_bgmHandle, true);
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

    UnloadAudio(m_bgmHandle);

    m_levelManager = nullptr;
}

void TitleScene::Update()
{
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Update();
    }

    ProcessorM_Update(this);

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

        // スタートボタン押下
        if (m_startButtonMorningSeaBe->GetIsPressed()) {
            Manager_SetGameLevel(m_selectLevel);
            SetFade(60, { 0.0f,1.0f,1.0f,1.0f }, FADE_STATE::FADE_OUT, SCENE::SCENE_GAME);
        }

        // レベル選択
        bool changedLevel = false;
        if(m_leftButtonBe->GetIsPressed()) {
            m_selectLevel--;
            if (m_selectLevel < 1) {
                m_selectLevel = 2;
            }
            if (m_levelManager) {
                m_levelManager->SetLevelID(m_selectLevel);
            }
            changedLevel = true;
            
            
        }
        if (m_rightButtonBe->GetIsPressed()) {
            m_selectLevel++;
            if (m_selectLevel > 2) {
                m_selectLevel = 1;
            }
            if (m_levelManager) {
                m_levelManager->SetLevelID(m_selectLevel);
            }
            changedLevel = true;
           
        }

        if(changedLevel){
            if (m_selectLevel == 1) {
                m_cameraComp->SetClearColor({ 0.1f,0.7f,1.0f,1.0f });
                m_levelTextComp->SetText(u8"朝の海");
                m_levelDescTextComp->SetText(u8"比較的穏やかな朝の海。海賊モンスターも少なめで、砂金が集めやすいぞ。");
            }
            else {
                m_cameraComp->SetClearColor({ 0.2f, 0.3f, 0.3f,1.0f });
                m_levelTextComp->SetText(u8"夜の海");
                m_levelDescTextComp->SetText(u8"夜の海は早く抜けたい。船のスピードを上げよう…ということは砂金を集める時間も短いな。");
            }
        }
    }

    // 破棄予約されたGameObjectの収集
    this->CollectDestroyedGameObjects();
}

void TitleScene::Draw()
{
    ProcessorM_Draw(this);
}
