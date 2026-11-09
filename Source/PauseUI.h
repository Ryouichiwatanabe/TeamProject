#pragma once
#include "UI.h"
#include <string>

class OperationGuideUI;

class PauseUI : public UI
{
public:
    void Init() override;
    void Update() override;
   
    void OnPauseInput();    // ポーズキーが押されたとき（GameSceneから呼ぶ）

    void AddMenuContent(UI* _ui);	// メニュー表示中だけ見せたいUIを登録する

    // -- 「再開」ボタンのみ押せるように制限する設定 -- //
    void SetResumeOnly(bool _resumeOnly) { mbResumeOnly = _resumeOnly; }

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

     bool mbResumeOnly = false;      //trueの時は再開以外押せない
};