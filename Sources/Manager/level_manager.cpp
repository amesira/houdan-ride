#include "level_manager.h"

#include "Sources/Scene/scene_base.h"
#include "Sources/Core/game_object.h"
#include "Sources/Core/type_id.h"
#include "Sources/GameParts/factory.h"
#include "Sources/System/mi_fps.h"

#include <cstdlib>
#include <ctime>
#include <string>

#include "Sources/GameParts/Behavior/ball_behavior.h"
#include "Sources/GameParts/Behavior/enemy_behavior.h"
#include "Sources/GameParts/Behavior/player_behavior.h"
#include "Sources/GameParts/Behavior/train_behavior.h"

#include "Sources/GameParts/Component/camera_component.h"
#include "Sources/GameParts/Component/collider_component.h"
#include "Sources/GameParts/Component/transform_component.h"
#include "Sources/GameParts/Component/UiComponents/image_component.h"
#include "Sources/GameParts/Component/UiComponents/rect_transform_component.h"
#include "Sources/GameParts/Component/UiComponents/text_component.h"

#include "Sources/Content/fade.h"
#include "Sources/Manager/manager.h"
#include "Utility/mi_math.h"

LevelManagerBehavior::LevelManagerBehavior(GameObject* owner)
    : Behavior(BehaviorTypeID::getTypeID<LevelManagerBehavior>())
{
    srand(static_cast<unsigned int>(time(nullptr)));
}

void LevelManagerBehavior::ResetProgress()
{
    m_spawnIntervalZ = 40.0f;
    m_enemyInterval = 0.0f;
    m_goalTimer = 0.0f;
    m_goalReached = false;
    m_goalObjectsCreated = false;
    ApplyLevelSettings();
}

void LevelManagerBehavior::SetLevelID(int levelID)
{
    m_levelID = levelID;
    ApplyLevelSettings();
}

void LevelManagerBehavior::SetMapCamera(TransformComponent* transform, CameraComponent* camera)
{
    m_mapCameraTransform = transform;
    m_mapCamera = camera;
}

void LevelManagerBehavior::Update(IScene* pScene)
{
    SceneBase* scene = static_cast<SceneBase*>(pScene);
    if (!scene || !m_mainShip) return;

    const float shipPosZ = m_mainShip->GetPosition().z;
    m_enemyInterval -= FPS_GetDeltaTime();

    UpdateSpawn(scene, shipPosZ);
    UpdateBallRespawn(scene, shipPosZ);

    if (!m_isTitle) {
        UpdateMapCamera();
    }

    UpdateWater();
    UpdateGoal(scene, shipPosZ);
}

void LevelManagerBehavior::UpdateSpawn(SceneBase* pScene, float shipPosZ)
{
    if (m_spawnIntervalZ >= shipPosZ + 100.0f) return;

    XMFLOAT3 pos = {
        static_cast<float>(rand() % 20 - 10),
        -4.0f - static_cast<float>(rand() % 3),
        m_spawnIntervalZ
    };

    if (pos.x < 0.0f) pos.x -= 7.5f;
    else pos.x += 7.5f;

    int r = rand() % 20;
    if (r <= 7) {
        LevelObjects::CreateWoodboxes1(pScene, pos);

        pos = m_mainShip->GetPosition();
        pos.x += 4.0f;
        pos.z += 7.0f;
        if (m_enemyInterval <= 0.0f) {
            LevelObjects::CreateEnemyGroup1(pScene, pos);
            m_enemyInterval = 2.0f;
        }
    }
    else if (r <= 14) {
        LevelObjects::CreateWoodboxes2(pScene, pos);

        pos = m_mainShip->GetPosition();
        pos.x -= 4.0f;
        pos.z += 7.0f;
        if (m_enemyInterval <= 0.0f) {
            LevelObjects::CreateEnemyGroup1(pScene, pos);
            m_enemyInterval = 2.0f;
        }
    }
    else {
        LevelObjects::CreateGolfGame(pScene, pos, 0.0f);
    }

    m_spawnIntervalZ += 20.0f;
}

// 新しい砲弾を出現させる
void LevelManagerBehavior::UpdateBallRespawn(SceneBase* pScene, float shipPosZ)
{
    if (!m_mainBall) return;

    XMFLOAT3 ballPos = m_mainBall->GetPosition();
    float diff = MiMath::Distance(m_mainShip->GetPosition(), ballPos);
    if (diff <= 30.0f) return;

    m_mainBall->SetDestroyTimer(3.0f);

    GameObject* newBall = pScene->CreateGameObject();
    Factory::CreateBall(newBall, XMFLOAT3(0.0f, -0.5f, shipPosZ - 0.6f));
    m_mainBall = newBall->GetBehavior<BallBehavior>();
    m_mainBall->AddBounceVelocity(XMFLOAT3(0.0f, 5.0f, 3.0f));
}

// マップカメラの位置と向きを更新
void LevelManagerBehavior::UpdateMapCamera()
{
    if (!m_mapCameraTransform || !m_mapCamera) return;

    XMFLOAT3 shipPos = m_mainShip->GetPosition();
    m_mapCameraTransform->SetPosition(XMFLOAT3(shipPos.x, shipPos.y + 60.0f, shipPos.z - 30.0f));
    m_mapCamera->SetAtPosition(shipPos);
}

// 水面の位置を更新
void LevelManagerBehavior::UpdateWater()
{
    if (!m_waterTransform) return;

    XMFLOAT3 waterPos = m_waterTransform->GetPosition();
    XMFLOAT3 shipPos = m_mainShip->GetPosition();
    m_waterTransform->SetPosition(XMFLOAT3(shipPos.x, waterPos.y, shipPos.z));
}

// ゴールまでの距離表示とゴール到達後の処理
void LevelManagerBehavior::UpdateGoal(SceneBase* pScene, float shipPosZ)
{
    if (m_isTitle || !m_goalMeterText) return;

    bool oldGoalReached = m_goalReached;
    if (!m_goalReached) {
        float diff = m_goalDistance - shipPosZ;
        if (diff < 0.0f) {
            diff = 0.0f;
            m_goalReached = true;
        }
        if (diff < 100.0f && !m_goalObjectsCreated) {
            m_goalObjectsCreated = true;
            LevelObjects::CreateGoalObject(pScene, XMFLOAT3(0.0f, -5.0f, shipPosZ + 130.0f));
        }

        std::string meterText = "帰還まで残り: " + std::to_string(static_cast<int>(diff)) + " m";
        std::u8string u8MeterText = std::u8string(meterText.begin(), meterText.end());
        m_goalMeterText->SetText(u8MeterText);
    }

    if (!m_goalReached) return;

    std::string meterText = "船に帰還した！";
    std::u8string u8MeterText = std::u8string(meterText.begin(), meterText.end());
    m_goalMeterText->SetText(u8MeterText);
    m_goalMeterText->SetFontSize(100.0f);

    RectTransformComponent* rectComp = m_goalMeterText->GetOwner()->GetComponent<RectTransformComponent>();
    if (rectComp) {
        rectComp->SetPosition(XMFLOAT3(1280.0f / 2.0f, 300.0f, 0.0f));
    }

    if (!oldGoalReached) {
        m_mainShip->SetMoveSpeed(0.0f);
        m_goalTimer = 2.0f;
    }

    m_goalTimer -= FPS_GetUnscaledDeltaTime();
    if (m_goalTimer > 0.0f) return;

    m_goalTimer += 50.0f;
    SetFade(120, { 0.0f,1.0f,1.0f,1.0f }, FADE_STATE::FADE_OUT, SCENE::SCENE_RESULT);

    GameObject* player = pScene->GetGameObjectByName("Player");
    if (!player) return;

    PlayerBehavior* playerBe = player->GetBehavior<PlayerBehavior>();
    if (!playerBe) return;

    Manager_SendScore(static_cast<int>(playerBe->GetScore()));
}

void LevelManagerBehavior::ApplyLevelSettings()
{
    if (m_mainShip) {
        if (m_levelID == 1) {
            m_mainShip->SetMoveSpeed(2.5f);
        }
        else {
            m_mainShip->SetMoveSpeed(4.5f);
        }
    }

    if (!m_waterTransform) return;

    ImageComponent* imageComp = m_waterTransform->GetOwner()->GetComponent<ImageComponent>();
    if (!imageComp) return;

    if (m_levelID == 1) {
        imageComp->SetColor(XMFLOAT4(0.2f, 1.0f, 1.0f, 0.7f));
    }
    else {
        imageComp->SetColor(XMFLOAT4(0.0f, 0.2f, 0.2f, 0.7f));
    }
}

TrainBehavior* LevelObjects::CreateMainShip(SceneBase* pScene, XMFLOAT3 position)
{
    GameObject* ship = pScene->CreateGameObject();
    Factory::CreateTrain(ship, position);
    TrainBehavior* trainBe = ship->GetBehavior<TrainBehavior>();

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

    return trainBe;
}

void LevelObjects::CreateWoodboxes1(SceneBase* pScene, XMFLOAT3 position)
{
    GameObject* base = pScene->CreateGameObject();
    Factory::CreateDodai(base, { position.x, position.y - 5.0f, position.z });

    GameObject* woodbox = pScene->CreateGameObject();
    XMFLOAT3 woodboxPos = { position.x, position.y + 2.0f, position.z };
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
    GameObject* base = pScene->CreateGameObject();
    Factory::CreateDodai(base, { position.x, position.y - 5.0f, position.z });

    GameObject* woodbox = pScene->CreateGameObject();
    XMFLOAT3 woodboxPos = { position.x, position.y + 2.0f, position.z };
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
        enemyBe->AddForce(XMFLOAT3(6.0f, 15.0f, 0.0f));
    }
}

void LevelObjects::CreateGoalObject(SceneBase* pScene, XMFLOAT3 position)
{
    GameObject* goal = pScene->CreateGameObject();
    Factory::CreateGoalShip(goal, position);
}

void LevelObjects::CreateGolfGame(SceneBase* pScene, XMFLOAT3 position, float angleY)
{
    GameObject* golfGame = pScene->CreateGameObject();
    Factory::CreateGolfGameObject(golfGame, position, angleY);

    GameObject* collider = pScene->CreateGameObject();
    Factory::CreateBoxCollider(collider, position,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 2.5f, -2.5f),
        XMFLOAT3(6.0f, 6.0f, 0.7f));

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
