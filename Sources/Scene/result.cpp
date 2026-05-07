#include "result.h"

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
#include "Sources/GameParts/Behavior/button_behavior.h"
#include "Sources/GameParts/Component/UiComponents/text_component.h"

#include <vector>
#include <algorithm>  // std::shuffle
#include <random>     // std::mt19937

#include "Sources/System/mi_fps.h"

#include "Sources/GameParts/Component/camera_component.h"


struct ResultDef
{
    int value;
    enum { CHEST, BOTTLE } type;
    XMFLOAT3 scale;
};

ResultDef g_ResultDefs[] =
{
    { 5000, ResultDef::CHEST,  XMFLOAT3(2.6f, 2.6f, 2.6f) }, // 0
    { 1000, ResultDef::CHEST,  XMFLOAT3(2.2f, 2.2f, 2.2f) }, // 1
    {  500, ResultDef::BOTTLE, XMFLOAT3(2.9f, 2.9f, 2.9f) }, // 2
    {  100, ResultDef::BOTTLE, XMFLOAT3(2.6f, 2.6f, 2.6f) }, // 3
};

std::vector<int> BuildResultIndexList(int score)
{
    std::vector<int> result;
    int counter = 0;

    while (score > 0){
        // 5000　最大10個
        if (score >= 5000 && counter < 15) {
            result.push_back(0);
            score -= 5000;
            counter++;
            continue;
        }

        // 1000 or 500×2
        if (score >= 1000) {
            // 1000ちょうど or 余裕があるならランダム
            if (score >= 1000 && (rand() % 5 >= 3))
            {
                result.push_back(2); // 500
                result.push_back(2); // 500
                score -= 1000;
            }
            else
            {
                result.push_back(1); // 1000
                score -= 1000;
            }
            continue;
        }

        // 500
        if (score >= 300) {
            result.push_back(2);
            score -= 300;
            continue;
        }

        // 100
        result.push_back(3);
        score -= 100;
    }

    return result;
}


void ResultScene::Initialize()
{
    this->Reset();

    ProcessorM_Initialize();

    FPS_SetTimeScale(1.0f);

    // camera
    GameObject* camera = this->CreateGameObject();
    Factory::CreateTpsCamera(camera, { 0.0f,3.0f,0.0f }, { 0.0f,0.0f,0.0f });
    TpsCameraBehavior* tpsCameraBe = camera->GetBehavior<TpsCameraBehavior>();
    tpsCameraBe->SetFreeze(true);
    tpsCameraBe->SetCameraAtOffset({ 0.0f,5.0f,10.0f });
    tpsCameraBe->SetCameraPosOffset({ 30.0f,2.0f,10.0f });

    if (m_levelID == 2) {
        CameraComponent* cameraComp = camera->GetComponent<CameraComponent>();
        cameraComp->SetClearColor({ 0.2f, 0.3f, 0.3f,1.0f });
    }

    // light
    GameObject* light = this->CreateGameObject();
    Factory::CreateLight(light, { 0.5f,-1.0f,0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.65f,0.65f,0.65f,1.0f });

    // 水面を生成
    GameObject* water = this->CreateGameObject();
    water->SetName("Water");
    Factory::CreateUiImageWorld(water, XMFLOAT3(0.0f, -3.0f, 0.0f), XMFLOAT3(XMConvertToRadians(-90.0f), 0.0f, 0.0f), XMFLOAT3(100.0f, 200.0f, 1.0f));
    ImageComponent* imageComp = water->GetComponent<ImageComponent>();
    imageComp->Load(L"asset\\Texture\\water.png");

    if(m_levelID == 1){
        imageComp->SetColor(XMFLOAT4(0.2f, 1.0f, 1.0f, 0.7f));
    }
    else {
        imageComp->SetColor(XMFLOAT4(0.0f, 0.2f, 0.2f, 0.7f));
    }

    // 船を生成
    // 船
    GameObject* ship = this->CreateGameObject();
    Factory::CreateTrain(ship, XMFLOAT3(0.0f,-5.0f,-2.0f));
   
    // goal
    GameObject* goal = this->CreateGameObject();
    Factory::CreateGoalShip(goal, XMFLOAT3(-10.0f, -5.0f, 20.0f));

    // player
    GameObject* player = this->CreateGameObject();
    Factory::CreatePlayer(player, { -2.0f,2.0f,-3.0f });

    // platform
    GameObject* platform = this->CreateGameObject();
    Factory::CreatePlatform(platform, XMFLOAT3(-20.0f, -4.0f, -2.0f), XMFLOAT3(3.0f, 3.0f, 6.0f));

    // ui
    GameObject* uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 1280.0f / 2.0f - 620.0f, 100.0f, 0.0f }, u8"結果発表！", 70.0f, { 1.0f,1.0f,1.0f,1.0f }, false);

    uiText = this->CreateGameObject();
    Factory::CreateUiText(uiText, { 1280.0f / 2.0f - 620.0f, 200.0f, 0.0f }, u8"集めた砂金の数: 000000 G", 50.0f, { 1.0f,1.0f,1.0f,1.0f }, false);
    m_scoreTextComp = uiText->GetComponent<TextComponent>();

    // スコアの値から、生成するスコアオブジェクト群を決定
    m_scoreObjectIDs = BuildResultIndexList(m_score);

    std::mt19937 rng(1234);
    std::shuffle(m_scoreObjectIDs.begin(), m_scoreObjectIDs.end(), rng);

    m_timer = 0.0f;
    m_counter = 0;

    // 生成完了のタイミングでスコアが確定するように調整
    m_scoreTimerMax = 0.1f * m_scoreObjectIDs.size();
    m_scoreTimer = 0.0f;

    m_bgmHandle = LoadAudio("asset\\Audio\\result.wav");
    PlayAudio(m_bgmHandle, true);
}

void ResultScene::Finalize()
{
    ProcessorM_Finalize();
    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Destroy();
    }

    UnloadAudio(m_bgmHandle);

    m_scoreTextComp = nullptr;
    m_titleButtonBehavior = nullptr;
}

void ResultScene::Update()
{
    m_timer += FPS_GetDeltaTime();
    if (m_timer > 0.1f) {
        if(m_counter < m_scoreObjectIDs.size()){
            int index = m_scoreObjectIDs[m_counter];
            ResultDef& def = g_ResultDefs[index];
            GameObject* scoreObj = this->CreateGameObject();

            static const XMFLOAT3 basePosition = XMFLOAT3(-20.0f, -4.0f, -2.0f);
            XMFLOAT3 position = XMFLOAT3(
                basePosition.x + (rand() % 10) - 5.0f,
                basePosition.y + 20.0f,
                basePosition.z + (rand() % 10) - 5.0f);
            XMFLOAT3 rotation = XMFLOAT3((float)(rand() % 360), (float)(rand() % 360), (float)(rand() % 360));

            if (def.type == ResultDef::CHEST) {
                Factory::CreateResultChest(scoreObj, 
                    position,rotation,def.scale);
            }
            else if (def.type == ResultDef::BOTTLE) {
                Factory::CreateResultBottle(scoreObj, 
                    position, rotation, def.scale);
            }
            m_counter++;
            m_timer = 0.0f;
        }
    }

    // スコアテキスト更新
    float dispScoreValue = static_cast<float>(m_score);
    if(m_scoreTimer <= m_scoreTimerMax){
        dispScoreValue *= (m_scoreTimer / m_scoreTimerMax);
        m_scoreTimer += FPS_GetDeltaTime();

        // 最終的に正確な値にする
        if(m_scoreTimer > m_scoreTimerMax){
            dispScoreValue = static_cast<float>(m_score);

            // ノルマ達成かどうか
            GameObject* uiText = this->CreateGameObject();
            if (m_score >= 30000){
                Factory::CreateUiText(uiText, { 1280.0f / 2.0f - 620.0f, 300.0f, 0.0f }, u8"ノルマ達成！", 70.0f, { 1.0f,1.0f,0.0f,1.0f }, false);
            }
            else {
                Factory::CreateUiText(uiText, { 1280.0f / 2.0f - 620.0f, 300.0f, 0.0f }, u8"ノルマ未達成...", 70.0f, { 1.0f,0.0f,0.0f,1.0f }, false);
            }

            GameObject* uiButton = this->CreateGameObject();
            Factory::CreateUiButton(uiButton, { 300, 650.0f }, { 300.0f,80.0f }, { 1.0f,1.0f,1.0f,1.0f });
            m_titleButtonBehavior = uiButton->GetBehavior<ButtonBehavior>();

            uiText = this->CreateGameObject();
            Factory::CreateUiText(uiText, { 300.0f, 660.0f, 0.0f }, u8"タイトルへ", 50.0f, { 0.0f,0.8f,1.0f,1.0f }, true);
           
        }

        std::string s = "集めた砂金: ";
        int value = static_cast<int>(dispScoreValue);
        for (int i = 0; i < 6; i++) {
            value /= 10;
            if (value == 0) {
                s += "0";
            }
        }
        s += std::to_string(static_cast<int>(dispScoreValue));
        s += " G";

        std::u8string u8 = std::u8string(s.begin(), s.end());
        m_scoreTextComp->SetText(u8);
    }

    // タイトルボタン処理
    if(m_titleButtonBehavior){
        if (m_titleButtonBehavior->GetIsPressed()) {
            SetFade(60, { 0.0f,1.0f,1.0f,1.0f }, FADE_STATE::FADE_OUT, SCENE::SCENE_TITLE);
        }
    }

    std::vector<GameObject>& gameObjects = this->GetGameObjects();
    for (GameObject& obj : gameObjects) {
        obj.Update();
    }

    ProcessorM_Update(this);


    // 破棄予約されたGameObjectの収集
    this->CollectDestroyedGameObjects();
}

void ResultScene::Draw()
{
    ProcessorM_Draw(this);
}
