#pragma once

class IScene;
class SceneBase;

// レベル管理関連
void LevelM_Initialize(SceneBase* pScene);
void LevelM_Finalize();
void LevelM_Update(SceneBase* pScene);
