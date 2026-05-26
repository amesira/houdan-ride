//===================================================
// game.cpp [ゲームシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//===================================================
#include "game.h"

#include "Sources/System/Device/Audio.h"
#include "Sources/Manager/processor_manager.h"

#include "Sources/GameParts/factory.h"
#include "Sources/Manager/particle_manager.h"
#include "Sources/System/Device/keyboard.h"
#include "Sources/Manager/level_manager.h"
#include "Sources/GameParts/Behavior/button_behavior.h"
#include "Sources/GameParts/Behavior/tps_camera_behavior.h"
#include "Sources/GameParts/Component/UiComponents/text_component.h"
#include "Sources/GameParts/Component/UiComponents/image_component.h"
#include "Sources/GameParts/Component/camera_component.h"
#include "Sources/GameParts/Component/transform_component.h"
#include "Sources/GameParts/Behavior/ball_behavior.h"
#include "Sources/GameParts/Behavior/train_behavior.h"

#include "Sources/Content/fade.h"
#include "Sources/System/mi_fps.h"

//===================================================
// ゲームシーン初期化処理
//===================================================
void GameScene::Initialize()
{
    this->Reset();

    // Processor初期化
    ProcessorM_Initialize();

    // TimeScaleをリセット
    FPS_SetTimeScale(1.0f);

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateTpsCamera(camera, { 0.0f,3.0f,0.0f }, { 0.0f,5.0f,0.0f });
    m_tpsCamera = camera->GetBehavior<TpsCameraBehavior>();
    if (m_level == 2) {
        CameraComponent* cameraComp = camera->GetComponent<CameraComponent>();
        cameraComp->SetClearColor({ 0.2f, 0.3f, 0.3f,1.0f });
    }

    // skybox作成テスト
    GameObject* skybox = this->CreateGameObject();
    Factory::CreateModel(skybox, "asset\\Model\\skybox.fbx", { 0.0f,0.0f,0.0f }, { XMConvertToRadians(-90.0f),0.0f,0.0f}, {50.0f,50.0f,50.0f});
    
    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateLight(light, { 0.5f,-1.0f,0.5f,0.0f }, { 0.1f,0.1f,0.1f,1.0f }, { 0.7f,0.7f,0.7f,1.0f });

    GameObject* player = this->CreateGameObject();
    Factory::CreatePlayer(player, { -2.0f,2.0f,-3.0f });

    // ui
    {
        GameObject* uiText = nullptr;

        // スコアテキスト
        /*GameObject* uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 10.0f, 50.0f, 0.0f }, u8"集めた砂金: 0000000 G", 30.0f, { 1.0f,1.0f,1.0f,1.0f },false);
        uiText->SetName("ScoreText");

        GameObject* scoreMeter = this->CreateGameObject();
        Factory::CreateUiImage(scoreMeter, { 40.0f, 80.0f, 0.0f }, 0.0f, { 100.0f,80.0f }, L"asset\\Texture\\gold.png");*/

        /*uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 10.0f, 100.0f, 0.0f }, u8"ノルマは 30000G !", 20.0f, { 1.0f,1.0f,0.0f,1.0f }, false);*/

        uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 350.0f, 50.0f, 0.0f }, u8"-500", 30.0f, { 1.0f,0.0f,0.0f,1.0f }, false);
        uiText->SetName("PenaltyText");

        // タイトルへ戻るボタン・ヘルプボタン
        GameObject* uiButton = this->CreateGameObject();
        Factory::CreateUiButton(uiButton, { 1280.0f - 50.0f, 720.0f - 50.0f }, { 60.0f,60.0f }, {1.0f,1.0f,1.0f,1.0f}, L"asset\\Texture\\door.png");
        m_backTitleButton = uiButton->GetBehavior<ButtonBehavior>();
        uiButton = this->CreateGameObject();
        Factory::CreateUiButton(uiButton, { 1280.0f - 100.0f, 720.0f - 50.0f }, { 60.0f,60.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"asset\\Texture\\help.png");
        m_helpButton = uiButton->GetBehavior<ButtonBehavior>();

        GameObject* uiSlider = this->CreateGameObject();
        Factory::CreateUiSlider(uiSlider, { 400.0f, 500.0f, 0.0f }, 0.0f, { 100.0f,20.0f });
        uiSlider->SetName("ThrowPowerSlider");

        uiText = this->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 200.0f, 0.0f }, u8" ", 100.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
        m_startText = uiText->GetComponent<TextComponent>();
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

    GameObject* mapCamera = this->CreateGameObject();
    Factory::CreateMapCamera(mapCamera, { 0.0f,20.0f,0.0f }, { 0.0f,0.0f,0.0f });

    // Goal Meter
    GameObject* goalMeter = this->CreateGameObject();
    Factory::CreateUiText(goalMeter, { 1280.0f / 2.0f, 45.0f, 0.0f }, u8"帰還まで残り: 1000 m", 36.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
    GameObject* goalMeterSlider = this->CreateGameObject();
    Factory::CreateUiSlider(goalMeterSlider, { 1280.0f / 2.0f, 65.0f, 0.0f }, 0.0f, { 600.0f,20.0f });
    
    // LevelManager
    GameObject* levelManagerObj = this->CreateGameObject();
    levelManagerObj->SetName("LevelManager");
    LevelManagerBehavior* levelManager = levelManagerObj->AddBehavior<LevelManagerBehavior>();
    levelManager->SetTitleMode(false);
    levelManager->SetWater(water->GetComponent<TransformComponent>());
    levelManager->SetMainShip(ship);
    levelManager->SetMainBall(ball->GetBehavior<BallBehavior>());
    levelManager->SetMapCamera(mapCamera->GetComponent<TransformComponent>(), mapCamera->GetComponent<CameraComponent>());
    levelManager->SetGoalMeter(goalMeter->GetComponent<TextComponent>());
    levelManager->SetLevelID(m_level);
    levelManager->ResetProgress();

    // チュートリアル
    {
        GameObject* tutorialUi = this->CreateGameObject();
        Factory::CreateUiImage(tutorialUi, { 1280.0f / 2.0f, 720.0f / 2.0f, 0.0f }, 0.0f, { 1280.0f,720.0f }, L"asset\\Texture\\white.bmp");
        ImageComponent* imageComp = tutorialUi->GetComponent<ImageComponent>();
        imageComp->SetColor({ 0.0f,0.0f,0.0f,0.6f });
        tutorialUi->SetName("TutorialUI_Panel");
        m_tutorialPages.push_back(tutorialUi);

        tutorialUi = this->CreateGameObject();
        Factory::CreateUiImage(tutorialUi, { 1280.0f / 2.0f, 720.0f / 2.0f, 0.0f }, 0.0f, { 1280.0f * 0.8f,720.0f * 0.8f }, L"asset\\Texture\\tutorial_1.png");
        tutorialUi->SetName("TutorialUI_Image");
        m_tutorialPages.push_back(tutorialUi);
        m_tutorialImage = tutorialUi->GetComponent<ImageComponent>();

        tutorialUi = this->CreateGameObject();
        Factory::CreateUiButton(tutorialUi, { 1280.0f / 2.0f - 500.0f, 720.0f / 2.0f }, { 100.0f,50.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"asset\\Texture\\left.png");
        m_tutorialLeftButton = tutorialUi->GetBehavior<ButtonBehavior>();
        m_tutorialPages.push_back(tutorialUi);

        tutorialUi = this->CreateGameObject();
        Factory::CreateUiButton(tutorialUi, { 1280.0f / 2.0f + 500.0f, 720.0f / 2.0f }, { 100.0f,50.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"asset\\Texture\\right.png");
        m_tutorialRightButton = tutorialUi->GetBehavior<ButtonBehavior>();
        m_tutorialPages.push_back(tutorialUi);

        tutorialUi = this->CreateGameObject();
        Factory::CreateUiText(tutorialUi, { 1280.0f / 2.0f, 720.0f / 2.0f + 300.0f, 0.0f }, u8"Enterで終了", 50.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
        tutorialUi->SetName("TutorialUI_EnterText");
        m_tutorialPages.push_back(tutorialUi);
    }

    if (m_tutorialStep < 0) {
        for (GameObject* page : m_tutorialPages) {
            page->SetActive(false);
        }
    }

    // pointer
    GameObject* pointer = this->CreateGameObject();
    Factory::CreatePointer(pointer);

    m_startTimer = 4.0f;
    m_isPlaying = false;

    m_bgmHandle = LoadAudio("asset\\Audio\\game.wav");
    PlayAudio(m_bgmHandle, true);
}

//===================================================
// ゲームシーン終了処理
//===================================================
void GameScene::Finalize()
{
    ProcessorM_Finalize();
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Destroy();
    }

    UnloadAudio(m_bgmHandle);
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

    // チュートリアル操作
    if(m_tutorialStep >= 0){
        // 表示終了
        if (Keyboard_IsKeyDownTrigger(KK_ENTER)) {
            m_tutorialStep = -1;
            for(GameObject* page : m_tutorialPages){
                page->SetActive(false);
            }

            if(m_startTimer <= 0.0f){
                m_isPlaying = true;
                m_tpsCamera->UseSlowMotion(true);
            }
        }

        bool input = false;
        if(m_tutorialLeftButton->GetIsPressed()){
            m_tutorialStep--;
            if(m_tutorialStep < 0){
                m_tutorialStep = 0;
            }
            input = true;
        }
        if (m_tutorialRightButton->GetIsPressed()) {
            m_tutorialStep++;
            if (m_tutorialStep > 2) {
                m_tutorialStep = 2;
            }
            input = true;
        }

        if (input) {
            switch (m_tutorialStep) {
            case 0:
                m_tutorialImage->Load(L"asset\\Texture\\tutorial_1.png");
                break;
            case 1:
                m_tutorialImage->Load(L"asset\\Texture\\tutorial_2.png");
                break;
            case 2:
                m_tutorialImage->Load(L"asset\\Texture\\tutorial_3.png");
                break;
            default:
                break;
            }
        }
    }
    else{

        if(m_startTimer > 0.0f){
            m_startTimer -= FPS_GetUnscaledDeltaTime();

            if (m_startText && m_startTimer <= 0.0f) {
                m_startText->SetText(u8"");
            }
            else if (m_startTimer <= 1.0f) {
                m_isPlaying = true;
                m_tpsCamera->UseSlowMotion(true);
                m_startText->SetText(u8"GO!");
            }
            else {
                std::string countStr = std::to_string(static_cast<int>(m_startTimer));
                std::u8string u8 = std::u8string(countStr.begin(), countStr.end());
                m_startText->SetText(u8);
            }
        }
    }
    if (m_backTitleButton->GetIsPressed()) {
        SetFade(60, { 0.0f,1.0f,1.0f,1.0f }, FADE_STATE::FADE_OUT, SCENE::SCENE_TITLE);
    }
    if (m_backTitleButton->GetIsHover()) {
        FPS_SetTimeScale(0.5f);
    }

    if (m_helpButton->GetIsPressed()) {
        m_isPlaying = false;
        m_tpsCamera->UseSlowMotion(false);

        m_tutorialStep = 0;
        for (GameObject* page : m_tutorialPages) {
            page->SetActive(true);
        }
    }

    if (m_isPlaying) {

    }
    else {
        FPS_SetTimeScale(0.01f);
    }

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
