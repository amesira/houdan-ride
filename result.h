#pragma once
#include "scene_base.h"
#include <vector>

class TextComponent;
class ButtonBehavior;

// リザルトシーン
class ResultScene : public SceneBase {
private:
    float m_timer;
    int m_counter;

    TextComponent* m_scoreTextComp;
    float m_scoreTimerMax;
    float m_scoreTimer;

    int m_score;
    std::vector<int> m_scoreObjectIDs;

    ButtonBehavior* m_titleButtonBehavior;

public:
    void    Initialize() override;
    void    Finalize() override;

    void    Update() override;
    void    Draw() override;
};