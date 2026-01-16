#pragma once
#include "scene_base.h"
#include <vector>

class TextComponent;
class ButtonBehavior;

// リザルトシーン
class ResultScene : public SceneBase {
private:
    int m_levelID;

    float m_timer;
    int m_counter;

    TextComponent* m_scoreTextComp;
    float m_scoreTimerMax;
    float m_scoreTimer;

    int m_score;
    std::vector<int> m_scoreObjectIDs;

    ButtonBehavior* m_titleButtonBehavior;

    int m_bgmHandle;

public:
    void    Initialize() override;
    void    Finalize() override;

    void    Update() override;
    void    Draw() override;

    void SetScore(int score) { m_score = score; }
    void SetLevelID(int levelID) { m_levelID = levelID; }
};