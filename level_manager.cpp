#include "level_manager.h"
#include "scene_interface.h"
#include "game_object.h"
#include "factory.h"
#include "fps.h"
#include "scene_base.h"

#include <iostream>

static float g_LevelM_Timer = 0.0f;

void LevelM_Initialize(SceneBase* pScene)
{
    srand(time(NULL));
    int angle = 60;

    GameObject* cube = nullptr;
    for (int i = 0; i < 10; i++) {
        cube = pScene->CreateGameObject();
        XMFLOAT3 posOffset = { static_cast<float>(rand() % 5),0.0f,static_cast<float>(rand() % 5) };
        
        XMFLOAT3 rot = { static_cast<float>(rand() % angle - angle / 2), static_cast<float>(rand() % angle - angle / 2),static_cast<float>(rand() % angle - angle / 2) };
        rot.x = XMConvertToRadians(rot.x);
        rot.y = XMConvertToRadians(rot.y);
        rot.z = XMConvertToRadians(rot.z);
        XMFLOAT4 color = { static_cast<float>(rand() % 100) / 100.0f,static_cast<float>(rand() % 100) / 100.0f,static_cast<float>(rand() % 100) / 100.0f,1.0f };

        Factory::CreateBox(
            cube, 
            { 0.0f + i * 7.0f + posOffset.x,0.0f,5.0f + posOffset.z },
            rot,
            { 7.0f,1.0f,7.0f }, 
            color);
    }
}

void LevelM_Finalize()
{
}

void LevelM_Update(IScene* pScene)
{
    g_LevelM_Timer += FPS_GetDeltaTime();
}
