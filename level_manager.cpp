#include "level_manager.h"
#include "scene_interface.h"
#include "game_object.h"
#include "factory.h"
#include "mi_fps.h"
#include "scene_base.h"

#include <iostream>

#include "train_behavior.h"
#include "liftup_behavior.h"
#include "ball_behavior.h"
#include "enemy_behavior.h"

#include "transform_component.h"
#include "image_component.h"
#include "camera_component.h"
#include "model_component.h"
#include "collider_component.h"
#include "text_component.h"
#include "rect_transform_component.h"
#include "player_behavior.h"

#include "mi_math.h"

#include "fade.h"

static float g_LevelM_Timer = 0.0f;
static float g_SpawnIntervalZ = 0.0f;

static TrainBehavior* g_MainShip_TrainBehavior = nullptr;
static BallBehavior* g_MainBall_BallBehavior = nullptr;

static TransformComponent* g_MapCamera_Transform = nullptr;
static CameraComponent* g_MapCamera_CameraComp = nullptr;

static TransformComponent* g_Water_Transform = nullptr;

static bool g_isTitle = false;

static TextComponent* g_GoalMeter_TextComp = nullptr;
static float g_GoalDistance = 200.0f;
static bool g_GoalReached = false;
static bool g_CreateGoalObjects = false;

static float g_EnemyInterval = 0.0f;

static float g_Timer = 0.0f;

void LevelM_Initialize(SceneBase* pScene, bool isTitle)
{
    g_isTitle = isTitle;

    srand(time(NULL));

    // 水面を生成
    GameObject* water = pScene->CreateGameObject();
    water->SetName("Water");
    Factory::CreateUiImageWorld(water, XMFLOAT3(0.0f, -3.0f, 0.0f), XMFLOAT3(XMConvertToRadians(-90.0f), 0.0f, 0.0f), XMFLOAT3(100.0f, 200.0f, 1.0f));
    ImageComponent* imageComp = water->GetComponent<ImageComponent>();
    imageComp->Load(L"asset\\Texture\\water.png");
    imageComp->SetColor(XMFLOAT4(0.2f, 1.0f, 1.0f, 0.7f));
    g_Water_Transform = water->GetComponent<TransformComponent>();

    // 船を生成
    LevelObjects::CreateMainShip(pScene, XMFLOAT3(0.0f, -5.0f, -2.0f));
    g_MainShip_TrainBehavior->SetMoveSpeed(2.5f); // スピード上がると難易度上がる

    // ボールを生成
    GameObject* ball = pScene->CreateGameObject();
    Factory::CreateBall(ball, { 0.0f,5.0f,0.0f });
    g_MainBall_BallBehavior = ball->GetBehavior<BallBehavior>();

    if(!g_isTitle){ // ゲーム
        // マップカメラ
        GameObject* camera = pScene->CreateGameObject();
        Factory::CreateMapCamera(camera, { 0.0f,20.0f,0.0f }, { 0.0f,0.0f,0.0f });
        g_MapCamera_Transform = camera->GetComponent<TransformComponent>();
        g_MapCamera_CameraComp = camera->GetComponent<CameraComponent>();

        // 帰還するまでのメートルを示すUI
        GameObject* uiText = pScene->CreateGameObject();
        Factory::CreateUiText(uiText, { 1280.0f / 2.0f, 60.0f, 0.0f }, u8"帰還まで: 1000 m", 50.0f, { 1.0f,1.0f,1.0f,1.0f }, true);
        g_GoalMeter_TextComp = uiText->GetComponent<TextComponent>();

        g_GoalReached = false;
        g_CreateGoalObjects = false;
    }
    else { // タイトル
        
    }

    g_SpawnIntervalZ = 40.0f;
    g_Timer = 0.0f;
}

void LevelM_Finalize()
{
    g_MainShip_TrainBehavior = nullptr;
    g_MainBall_BallBehavior = nullptr;

    g_MapCamera_Transform = nullptr;
    g_MapCamera_CameraComp = nullptr;

    g_Water_Transform = nullptr;

    g_GoalMeter_TextComp = nullptr;
    g_GoalReached = false;
    g_CreateGoalObjects = false;
}

void LevelM_Update(SceneBase* pScene)
{
    float shipPosZ = g_MainShip_TrainBehavior->GetPosition().z;

    g_EnemyInterval -= FPS_GetDeltaTime();

    // 一定間隔で木箱を生成
    if (g_SpawnIntervalZ < shipPosZ + 100.0f){

        // ランダムな位置を生成
        XMFLOAT3 pos = {
            static_cast<float>(rand() % 20 - 10),
            -4.0f - static_cast<float>(rand() % 3),
            g_SpawnIntervalZ
        };

        if (pos.x < 0.0f) pos.x -= 7.5f;
        else pos.x += 7.5f;

        // ランダムで木箱を生成
        int r = rand() % 20;
        if (r <= 7) {
            LevelObjects::CreateWoodboxes1(pScene, pos);

            pos = g_MainShip_TrainBehavior->GetPosition();
            pos.x += 4.0f;
            pos.z += 7.0f;
            LevelObjects::CreateEnemyGroup1(pScene, pos);
        }
        else if (r <= 14) {
            LevelObjects::CreateWoodboxes2(pScene, pos);

            pos = g_MainShip_TrainBehavior->GetPosition();
            pos.x -= 4.0f;
            pos.z += 7.0f;
            LevelObjects::CreateEnemyGroup1(pScene, pos);
        }
        else {
            LevelObjects::CreateGolfGame(pScene, pos,0.0f);
        }


        g_SpawnIntervalZ += 20.0f;
    }

    // ボール処理
    if (g_MainBall_BallBehavior) {
        XMFLOAT3 ballPos = g_MainBall_BallBehavior->GetPosition();
        float diff = MiMath::Distance(
            g_MainShip_TrainBehavior->GetPosition(),
            ballPos
        );

        if (diff > 30.0f) {
            g_MainBall_BallBehavior->SetDestroyTimer(3.0f);

            GameObject* newBall = pScene->CreateGameObject();
            Factory::CreateBall(newBall, XMFLOAT3(0.0f, -0.5f, shipPosZ - 0.6f));
            g_MainBall_BallBehavior = newBall->GetBehavior<BallBehavior>();
            g_MainBall_BallBehavior->AddBounceVelocity(XMFLOAT3(0.0f, 5.0f, 3.0f));
        }
    }

    if(!g_isTitle){
        // マップカメラ追従
        g_MapCamera_Transform->SetPosition(XMFLOAT3(
            g_MainShip_TrainBehavior->GetPosition().x,
            g_MainShip_TrainBehavior->GetPosition().y + 60.0f,
            g_MainShip_TrainBehavior->GetPosition().z - 30.0f
        ));
        g_MapCamera_CameraComp->SetAtPosition(g_MainShip_TrainBehavior->GetPosition());
    }

    // 水面
    g_Water_Transform->SetPosition(XMFLOAT3(
        g_MainShip_TrainBehavior->GetPosition().x,
        g_Water_Transform->GetPosition().y,
        g_MainShip_TrainBehavior->GetPosition().z
    ));

    // 距離メーター更新
    bool oldGoalReached = g_GoalReached;
    if (g_GoalMeter_TextComp && !g_GoalReached) {
        float diff = g_GoalDistance - shipPosZ;
        if (diff < 0.0f) {
            diff = 0.0f;
            g_GoalReached = true;
        }
        if (diff < 100.0f && !g_CreateGoalObjects) {
            g_CreateGoalObjects = true;
            LevelObjects::CreateGoalObject(pScene, XMFLOAT3(0.0f, -5.0f, shipPosZ + 130.0f));
        }
        std::string meterText = "帰還まで: " + std::to_string(static_cast<int>(diff)) + " m";
        std::u8string u8MeterText = std::u8string(meterText.begin(), meterText.end());
        g_GoalMeter_TextComp->SetText(u8MeterText);
    }
    if (g_GoalReached) {
        std::string meterText = "本船へ帰還した！！";
        std::u8string u8MeterText = std::u8string(meterText.begin(), meterText.end());
        g_GoalMeter_TextComp->SetText(u8MeterText);
        g_GoalMeter_TextComp->SetFontSize(100.0f);
        RectTransformComponent* rectComp = g_GoalMeter_TextComp->GetOwner()->GetComponent<RectTransformComponent>();
        rectComp->SetPosition(XMFLOAT3(1280.0f / 2.0f, 300.0f, 0.0f));

        if (!oldGoalReached) {
            // ゴール到達時の処理
            g_MainShip_TrainBehavior->SetMoveSpeed(0.0f);
            g_Timer = 2.0f;
        }
        g_Timer -= FPS_GetUnscaledDeltaTime();
        if (g_Timer <= 0.0f) {
            g_Timer += 50.0f;
            SetFade(120, { 0.0f,1.0f,1.0f,1.0f }, FADE_STATE::FADE_OUT, SCENE::SCENE_RESULT);

            // 結果シーン用にスコアをセット
            GameObject* player = pScene->GetGameObjectByName("Player");
            PlayerBehavior* playerBe = player->GetBehavior<PlayerBehavior>();
            int score = static_cast<int>(playerBe->GetScore());
            Manager_SendScore(score);
        }
    }
}

void LevelM_ChangeLevel(SceneBase* pScene, int levelID)
{
    if(g_MainShip_TrainBehavior){
        if(levelID == 1){
            g_MainShip_TrainBehavior->SetMoveSpeed(2.5f);
        }
        else {
            g_MainShip_TrainBehavior->SetMoveSpeed(4.5f);
        }
    }

    if (g_Water_Transform) {
        ImageComponent* imageComp = g_Water_Transform->GetOwner()->GetComponent<ImageComponent>();
        if (levelID == 1) {
            imageComp->SetColor(XMFLOAT4(0.2f, 1.0f, 1.0f, 0.7f));
        }
        else {
            imageComp->SetColor(XMFLOAT4(0.0f, 0.2f, 0.2f, 0.7f));
        }
    }
}

void LevelObjects::CreateMainShip(SceneBase* pScene, XMFLOAT3 position)
{
    // 船
    GameObject* ship = pScene->CreateGameObject();
    Factory::CreateTrain(ship, position);
    TrainBehavior* trainBe = ship->GetBehavior<TrainBehavior>();

    // グローバル変数に格納
    g_MainShip_TrainBehavior = trainBe;

    // 船用子オブジェクト
    GameObject* collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position, 
        XMFLOAT3(0.0f, 0.0f, 0.0f), 
        XMFLOAT3(0.0f, 4.0f, 4.33f), 
        XMFLOAT3(5.4f, 1.0f, 3.0f));
    trainBe->AddChildCollider(collider);
    
    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, XMConvertToRadians(-47.0f), 0.0f),
        XMFLOAT3(-1.5f, 4.0f, 5.0f),
        XMFLOAT3(6.7f, 1.0f, 1.47f));
    trainBe->AddChildCollider(collider);
   
    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, XMConvertToRadians(45.0f), 0.0f),
        XMFLOAT3(2.13f, 4.0f, 4.52f),
        XMFLOAT3(6.7f, 1.0f, 1.47f));
    trainBe->AddChildCollider(collider);

    // 上デッキ
    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 5.58f, -5.5f),
        XMFLOAT3(3.0f, 3.15f, 3.2f));
    trainBe->AddChildCollider(collider);

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 5.58f, -8.4f),
        XMFLOAT3(9.7f, 3.15f, 3.2f));
    trainBe->AddChildCollider(collider);

    // 階段
    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(3.25f, 4.15f, -5.7f),
        XMFLOAT3(3.23f, 3.15f, 5.7f));
    trainBe->AddChildCollider(collider);

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(3.25f, 5.0f, -7.7f),
        XMFLOAT3(3.23f, 3.15f, 5.7f));
    trainBe->AddChildCollider(collider);

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(-3.24f, 4.15f, -5.7f),
        XMFLOAT3(3.23f, 3.15f, 5.7f));
    trainBe->AddChildCollider(collider);

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(-3.24f, 5.0f, -7.7f),
        XMFLOAT3(3.23f, 3.15f, 5.7f));
    trainBe->AddChildCollider(collider);
}

void LevelObjects::CreateWoodboxes1(SceneBase* pScene, XMFLOAT3 position)
{
    // 土台を作る
    GameObject* base = pScene->CreateGameObject();
    Factory::CreateDodai(base, { position.x, position.y - 5.0f, position.z });

    // 木箱を積む（縦に3つ）
    GameObject* woodbox = pScene->CreateGameObject();
    XMFLOAT3 woodboxPos = {
        position.x,
        position.y + 2.0f,
        position.z
    };
    Factory::CreateWoodbox(woodbox, woodboxPos);

    woodbox = pScene->CreateGameObject();
    woodboxPos.y += 2.0f;
    Factory::CreateWoodbox(woodbox, woodboxPos);

    woodbox = pScene->CreateGameObject();
    woodboxPos.y += 2.0f;
    Factory::CreateWoodbox(woodbox, woodboxPos);
}

void LevelObjects::CreateWoodboxes2(SceneBase* pScene, XMFLOAT3 position)
{
    // 土台を作る
    GameObject* base = pScene->CreateGameObject();
    Factory::CreateDodai(base, { position.x, position.y - 5.0f, position.z });

    // 木箱を積む（縦に3つ）
    GameObject* woodbox = pScene->CreateGameObject();
    XMFLOAT3 woodboxPos = {
        position.x,
        position.y + 2.0f,
        position.z
    };
    Factory::CreateWoodbox(woodbox, woodboxPos);

    woodbox = pScene->CreateGameObject();
    woodboxPos.x = position.x - 1.0f;
    woodboxPos.y += 4.0f;
    Factory::CreateWoodbox(woodbox, woodboxPos);

    woodbox = pScene->CreateGameObject();
    woodboxPos.x = position.x + 1.0f;
    Factory::CreateWoodbox(woodbox, woodboxPos);
}

void LevelObjects::CreateEnemyGroup1(SceneBase* pScene, XMFLOAT3 position)
{
    if(g_EnemyInterval > 0.0f)return;
    g_EnemyInterval = 2.0f;

    // 2もしくは3体の敵を生成
    int enemyCount = (rand() % 2) + 2;

    for (int i = 0; i < enemyCount; i++) {
        GameObject* enemy = pScene->CreateGameObject();
        XMFLOAT3 enemyPos = {
            position.x + static_cast<float>(rand() % 5 - 2),
            position.y,
            position.z + static_cast<float>(rand() % 5 - 2),
        };
        Factory::CreateEnemy(enemy, enemyPos);
        EnemyBehavior* enemyBe = enemy->GetBehavior<EnemyBehavior>();
        enemyBe->AddForce(XMFLOAT3(
            6.0f,
            15.0f,
            0.0f
        ));
    }
}

void LevelObjects::CreateGoalObject(SceneBase* pScene, XMFLOAT3 position)
{
    // ゴールオブジェクトを生成
    GameObject* goal = pScene->CreateGameObject();
    Factory::CreateGoalShip(goal, position);
}

void LevelObjects::CreateGolfGame(SceneBase* pScene, XMFLOAT3 position, float angleY)
{
    // ゴルフゲームオブジェクトを生成
    GameObject* golfGame = pScene->CreateGameObject();
    Factory::CreateGolfGameObject(golfGame, position, 180.0f);

    // 外枠
    GameObject* collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 2.5f, -2.5f),
        XMFLOAT3(6.0f,6.0f,0.7f));

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 2.5f, 2.5f),
        XMFLOAT3(6.0f, 6.0f, 0.7f));

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(2.5f, 2.5f, 0.0f),
        XMFLOAT3(0.7f, 6.0f, 6.0f));

    collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(-2.5f, 2.5f, 0.0f),
        XMFLOAT3(0.7f, 6.0f, 6.0f));

}
