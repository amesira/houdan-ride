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

#include "mi_math.h"

static float g_LevelM_Timer = 0.0f;
static float g_SpawnIntervalZ = 20.0f;

static TrainBehavior* g_MainShip_TrainBehavior = nullptr;
static BallBehavior* g_MainBall_BallBehavior = nullptr;

static TransformComponent* g_MapCamera_Transform = nullptr;
static CameraComponent* g_MapCamera_CameraComp = nullptr;

static TransformComponent* g_Water_Transform = nullptr;

void LevelM_Initialize(SceneBase* pScene)
{
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
    g_MainShip_TrainBehavior->SetMoveSpeed(1.5f);

    // ボールを生成
    GameObject* ball = pScene->CreateGameObject();
    Factory::CreateBall(ball, { 0.0f,5.0f,0.0f });
    g_MainBall_BallBehavior = ball->GetBehavior<BallBehavior>();

    // マップカメラ
    GameObject* camera = pScene->CreateGameObject();
    Factory::CreateMapCamera(camera, { 0.0f,20.0f,0.0f }, { 0.0f,0.0f,0.0f });
    g_MapCamera_Transform = camera->GetComponent<TransformComponent>();
    g_MapCamera_CameraComp = camera->GetComponent<CameraComponent>();
}

void LevelM_Finalize()
{

}

void LevelM_Update(SceneBase* pScene)
{
    float shipPosZ = g_MainShip_TrainBehavior->GetPosition().z;

    // 一定間隔で木箱を生成
    if (g_SpawnIntervalZ < shipPosZ + 50.0f){

        // ランダムな位置を生成
        XMFLOAT3 pos = {
            static_cast<float>(rand() % 20 - 10),
            -4.0f - static_cast<float>(rand() % 3),
            g_SpawnIntervalZ
        };

        if (pos.x < 0.0f) pos.x -= 7.5f;
        else pos.x += 7.5f;

        // ランダムで木箱を生成
        int r = rand() % 2;
        if (r == 0) {
            LevelObjects::CreateWoodboxes1(pScene, pos);
        }
        else if (r == 1) {
            LevelObjects::CreateWoodboxes2(pScene, pos);

            pos = g_MainShip_TrainBehavior->GetPosition();
            pos.x -= 4.0f;
            pos.z += 7.0f;
            LevelObjects::CreateEnemyGroup1(pScene, pos);
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

    // マップカメラ追従
    g_MapCamera_Transform->SetPosition(XMFLOAT3(
        g_MainShip_TrainBehavior->GetPosition().x,
        g_MainShip_TrainBehavior->GetPosition().y + 60.0f,
        g_MainShip_TrainBehavior->GetPosition().z - 30.0f
    ));
    g_MapCamera_CameraComp->SetAtPosition(g_MainShip_TrainBehavior->GetPosition());

    // 水面
    g_Water_Transform->SetPosition(XMFLOAT3(
        g_MainShip_TrainBehavior->GetPosition().x,
        g_Water_Transform->GetPosition().y,
        g_MainShip_TrainBehavior->GetPosition().z
    ));
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
    Factory::CreateBox(base, position, 
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(2.0f, 3.0f, 2.0f), 
        XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
    LiftupBehavior* liftBe = base->AddBehavior<LiftupBehavior>();

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
    Factory::CreateBox(base, position, 
        XMFLOAT3(0.0f, 0.0f, 0.0f), 
        XMFLOAT3(2.0f, 3.0f, 2.0f), 
        XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
    LiftupBehavior* liftBe = base->AddBehavior<LiftupBehavior>();

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