#include "level_manager.h"
#include "scene_interface.h"
#include "game_object.h"
#include "factory.h"
#include "mi_fps.h"
#include "scene_base.h"

#include <iostream>

#include "train_behavior.h"
#include "liftup_behavior.h"

#include "image_component.h"

static float g_LevelM_Timer = 0.0f;
static float g_SpawnIntervalZ = 20.0f;

static TrainBehavior* g_MainShip_TrainBehavior = nullptr;

void LevelM_Initialize(SceneBase* pScene)
{
    srand(time(NULL));

    // 水面を生成
    GameObject* water = pScene->CreateGameObject();
    water->SetName("Water");
    Factory::CreateUiImageWorld(water, XMFLOAT3(0.0f, -3.0f, 0.0f), XMFLOAT3(XMConvertToRadians(-90.0f), 0.0f, 0.0f), XMFLOAT3(100.0f, 200.0f, 1.0f));
    ImageComponent* imageComp = water->GetComponent<ImageComponent>();
    imageComp->Load(L"asset\\Texture\\white.bmp");
    imageComp->SetColor(XMFLOAT4(0.0f, 0.5f, 1.0f, 0.7f));

    // 船を生成
    LevelObjects::CreateMainShip(pScene, XMFLOAT3(0.0f, -5.0f, -2.0f));

    g_MainShip_TrainBehavior->SetMoveSpeed(1.5f);
}

void LevelM_Finalize()
{

}

void LevelM_Update(SceneBase* pScene)
{
    g_LevelM_Timer += FPS_GetDeltaTime();

    if (g_LevelM_Timer >= 3.0f) {
        g_LevelM_Timer = 0.0f;

        // 木箱生成
       // GameObject* woodbox = pScene->CreateGameObject();
        
        XMFLOAT3 pos = {
            static_cast<float>(rand() % 20 - 10),
            -4.0f -static_cast<float>(rand() % 3),
            g_SpawnIntervalZ
        };

        if (pos.x < 0.0f) {
            pos.x -= 7.5f;
        }
        else {
            pos.x += 7.5f;
        }
        //Factory::CreateWoodbox(woodbox, pos);

        LevelObjects::CreateWoodboxes2(pScene, pos);

        g_SpawnIntervalZ += 20.0f;
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
    Factory::CreateBox(base, position, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 3.0f, 2.0f), XMFLOAT4(0.5f, 0.35f, 0.05f, 1.0f));
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
    Factory::CreateBox(base, position, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 3.0f, 2.0f), XMFLOAT4(0.5f, 0.35f, 0.05f, 1.0f));
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
