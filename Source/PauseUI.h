#pragma once

#include "UI.h"

class PauseUI : public UI
{
public:
    PauseUI() = default;
    ~PauseUI() = default;

    void Init() override;
    void Update() override;
    void Draw() override;


    // -- 「再開」ボタンのみ押せるように制限する設定 -- //
    void SetResumeOnly(bool _resumeOnly) { mbResumeOnly = _resumeOnly; }

private:
    UI* mpResumeUI = nullptr;       // 「再開」UI
    UI* mpStageSelectUI = nullptr;  // 「ステージ選択へ」UI
    UI* mpTitleUI = nullptr;        // 「タイトルへ」UI
    UI* mpPauseOverlay = nullptr;   // オーバレイ

    bool mbResumeOnly = false;      //trueの時は再開以外押せない
};