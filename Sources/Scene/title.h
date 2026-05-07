//----------------------------------------------------
// title.h [タイトルシーン制御]
// 
// Author：Miu Kitamura
// Date  ：2025/09/05
//----------------------------------------------------
#ifndef TITLE_H
#define TITLE_H

#include "scene_base.h"
#include <vector>

class ButtonBehavior;
class CameraComponent;
class TextComponent;

// ゲームシーン
class TitleScene : public SceneBase {
private:
    bool m_isSelectScene = false;
    std::vector<GameObject*> m_titleUiObjects;
    std::vector<GameObject*> m_selectUiObjects;

    ButtonBehavior* m_startButtonMorningSeaBe = nullptr;

    int m_selectLevel = 1;
    ButtonBehavior* m_leftButtonBe = nullptr;
    ButtonBehavior* m_rightButtonBe = nullptr;

    CameraComponent* m_cameraComp = nullptr;
    TextComponent* m_levelTextComp = nullptr;
    TextComponent* m_levelDescTextComp = nullptr;

    int m_bgmHandle = -1;

public:
    void    Initialize() override;
    void    Finalize() override;

    void    Update() override;
    void    Draw() override;
};

#endif