#pragma once

#include "Sources/Core/behavior.h"
#include <DirectXMath.h>
using namespace DirectX;

class SceneBase;
class TrainBehavior;
class BallBehavior;
class TransformComponent;
class CameraComponent;
class TextComponent;

class LevelManagerBehavior : public Behavior {
private:
    bool m_isTitle = false;
    int m_levelID = 1;

    float m_spawnIntervalZ = 40.0f;
    float m_enemyInterval = 0.0f;
    float m_goalTimer = 0.0f;

    TrainBehavior* m_mainShip = nullptr;
    BallBehavior* m_mainBall = nullptr;

    TransformComponent* m_waterTransform = nullptr;
    TransformComponent* m_mapCameraTransform = nullptr;
    CameraComponent* m_mapCamera = nullptr;

    TextComponent* m_goalMeterText = nullptr;
    float m_goalDistance = 200.0f;
    bool m_goalReached = false;
    bool m_goalObjectsCreated = false;

public:
    LevelManagerBehavior(GameObject* owner);

    void Update(IScene* pScene) override;

    void SetTitleMode(bool isTitle) { m_isTitle = isTitle; }
    void SetLevelID(int levelID);
    void SetMainShip(TrainBehavior* ship) { m_mainShip = ship; }
    void SetMainBall(BallBehavior* ball) { m_mainBall = ball; }
    void SetWater(TransformComponent* water) { m_waterTransform = water; }
    void SetMapCamera(TransformComponent* transform, CameraComponent* camera);
    void SetGoalMeter(TextComponent* goalMeter) { m_goalMeterText = goalMeter; }
    void ResetProgress();

private:
    void UpdateSpawn(SceneBase* pScene, float shipPosZ);
    void UpdateBallRespawn(SceneBase* pScene, float shipPosZ);
    void UpdateMapCamera();
    void UpdateWater();
    void UpdateGoal(SceneBase* pScene, float shipPosZ);
    void ApplyLevelSettings();
};

namespace LevelObjects {
    TrainBehavior* CreateMainShip(SceneBase* pScene, XMFLOAT3 position);
    void CreateWoodboxes1(SceneBase* pScene, XMFLOAT3 position);
    void CreateWoodboxes2(SceneBase* pScene, XMFLOAT3 position);
    void CreateEnemyGroup1(SceneBase* pScene, XMFLOAT3 position);
    void CreateGoalObject(SceneBase* pScene, XMFLOAT3 position);
    void CreateGolfGame(SceneBase* pScene, XMFLOAT3 position, float angleY);
}
