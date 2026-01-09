#include "level_manager.h"
#include "scene_interface.h"
#include "game_object.h"
#include "factory.h"
#include "mi_fps.h"
#include "scene_base.h"

#include <iostream>

static float g_LevelM_Timer = 0.0f;
static float g_SpawnIntervalZ = 20.0f;

void LevelM_Initialize(SceneBase* pScene)
{
    srand(time(NULL));
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
        GameObject* woodbox = pScene->CreateGameObject();
        XMFLOAT3 pos = {
            static_cast<float>(rand() % 20 - 10),
            0.5f,
            g_SpawnIntervalZ
        };
        Factory::CreateWoodbox(woodbox, pos);

        g_SpawnIntervalZ += 20.0f;
    }
}