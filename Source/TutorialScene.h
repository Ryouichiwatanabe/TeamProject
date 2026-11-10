#pragma once
#include "Scene.h"
#include "UITypewriterText.h"

class Player;
class Goal;
class PlayerShapeUI;
class StageBackGround;
class PauseUI;
class UI;

class TutorialScene : public Scene
{
public:
    TutorialScene() = default;
    ~TutorialScene() override = default;

    void Initialize() override;
    void Update(float _deltaTime) override;
    void Draw() override;
    void Finalize() override;

private:
    void SetTutorialMessage(const char* _text);
    void UpdateTutorialStep(float _deltaTime);
    void UpdateShapeUIToggle();
    void UpdatePause();

private:
    // -- オブジェクト参照用 -- //
    Goal* mpGoal = nullptr;

    // -- UI -- //
    StageBackGround* mpBackgroundUI = nullptr;
    PlayerShapeUI* mpShapeUI = nullptr;
    PauseUI* mpPauseUI = nullptr;
    UI* mpSkipButton = nullptr;
    UI* mpMessageUI = nullptr;

    // -- チュートリアル進行管理 -- //
    int mnTutorialStep = 0;
    float mfStepTimer = 0.0f;
    UITypewriterText mTypewriter;
    int mMessageFontHandle = -1;

    // -- クリア演出用 -- //
    bool mbIsClear = false;
    float mfClearTimer = 0.0f;
};