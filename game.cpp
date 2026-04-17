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
#include "keyboard.h"
#include "level_manager.h"
#include "button_behavior.h"
#include "tps_camera_behavior.h"
#include "text_component.h"
#include "image_component.h"
#include "camera_component.h"

#include "fade.h"
#include "mi_fps.h"

//===================================================
// ゲームシーン初期化処理
//===================================================
void GameScene::Initialize()
{
    this->Reset();

    ProcessorM_Initialize();

    FPS_SetTimeScale(1.0f);

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateTpsCamera(camera, { 0.0f,3.0f,0.0f }, { 0.0f,5.0f,0.0f });
    m_tpsCamera = camera->GetBehavior<TpsCameraBehavior>();
    if (m_level == 2) {
        CameraComponent* cameraComp = camera->GetComponent<CameraComponent>();
        cameraComp->SetClearColor({ 0.2f, 0.3f, 0.3f,1.0f });
    }
    
    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateLight(light, { 0.5f,-1.0f,0.5f,0.0f }, { 0.1f,0.1f,0.1f,1.0f }, { 0.7f,0.7f,0.7f,1.0f });

    GameObject* player = this->CreateGameObject();
    Factory::CreatePlayer(player, { -2.0f,2.0f,-3.0f });

    // ui
    GameObject* uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 10.0f, 50.0f, 0.0f }, u8"集めた砂金: 0000000 G", 40.0f, { 1.0f,1.0f,1.0f,1.0f },false);
    uiText->SetName("ScoreText");

    uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 10.0f, 100.0f, 0.0f }, u8"ノルマは 30000G !", 30.0f, { 1.0f,1.0f,0.0f,1.0f }, false);

    uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 350.0f, 50.0f, 0.0f }, u8"-500", 30.0f, { 1.0f,0.0f,0.0f,1.0f }, false);
    uiText->SetName("PenaltyText");

    GameObject* uiSlider = this->CreateGameObject();
    Factory::CreateUiSlider(uiSlider, { 400.0f, 500.0f, 0.0f }, 0.0f, { 100.0f,20.0f });
    uiSlider->SetName("ThrowPowerSlider");

    GameObject* uiButton = this->CreateGameObject();
    Factory::CreateUiButton(uiButton, { 1280.0f - 50.0f, 50.0f }, { 60.0f,60.0f }, {1.0f,1.0f,1.0f,1.0f}, L"asset\\Texture\\door.png");
    m_backTitleButton = uiButton->GetBehavior<ButtonBehavior>();

    uiButton = this->CreateGameObject();
    Factory::CreateUiButton(uiButton, { 1280.0f - 100.0f, 50.0f }, { 60.0f,60.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"asset\\Texture\\help.png");
    m_helpButton = uiButton->GetBehavior<ButtonBehavior>();

    uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 200.0f, 0.0f }, u8" ", 100.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
    m_startText = uiText->GetComponent<TextComponent>();

    LevelM_Initialize(this);
    LevelM_ChangeLevel(this, m_level);

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
    LevelM_Finalize();

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

    LevelM_Update(this);

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
