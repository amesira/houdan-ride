#pragma once

class IScene;
class SceneBase;

#include <DirectXMath.h>
using namespace DirectX;

// レベル管理関連
void LevelM_Initialize(SceneBase* pScene, bool isTitle = false);
void LevelM_Finalize();
void LevelM_Update(SceneBase* pScene);

namespace LevelObjects {
    void CreateMainShip(SceneBase* pScene, XMFLOAT3 position);

    void CreateWoodboxes1(SceneBase* pScene, XMFLOAT3 position);
    void CreateWoodboxes2(SceneBase* pScene, XMFLOAT3 position);

    void CreateEnemyGroup1(SceneBase* pScene, XMFLOAT3 position);
}