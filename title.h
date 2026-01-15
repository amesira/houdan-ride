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

// ゲームシーン
class TitleScene : public SceneBase {
private:
    bool m_isSelectScene = false;
    std::vector<GameObject*> m_titleUiObjects;
    std::vector<GameObject*> m_selectUiObjects;

    ButtonBehavior* m_startButtonMorningSeaBe = nullptr;

public:
    void    Initialize() override;
    void    Finalize() override;

    void    Update() override;
    void    Draw() override;
};

#endif