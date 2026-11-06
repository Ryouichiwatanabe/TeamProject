#pragma once
#include "UI.h"
#include <string>

class OperationGuideUI;

class PauseUI : public UI
{
public:
    void Init() override;
    void Update() override;
    // void Draw() override;

    void OnPauseInput();    // ポーズキーが押されたとき（GameSceneから呼ぶ）

    void AddMenuContent(UI* _ui);	// メニュー表示中だけ見せたいUIを登録する

private:
    enum class State { Menu, Guide };

    void ChangeState(State _next);
    void UpdateMenu();
    void UpdateGuide();
    UI* CreateMenuButton(const std::string& _text, float _y);

private:
    State mState = State::Menu;

    UI* mpPauseOverlay = nullptr;
    UI* mpMenuRoot = nullptr;
    UI* mpResumeUI = nullptr;
    UI* mpStageSelectUI = nullptr;
    UI* mpTitleUI = nullptr;
    UI* mpOperationButton = nullptr;
    OperationGuideUI* mpGuideUI = nullptr;
};