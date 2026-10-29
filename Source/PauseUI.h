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

private:
    UI* mpResumeUI = nullptr;       // 「再開」UI
    UI* mpStageSelectUI = nullptr;  // 「ステージ選択へ」UI
    UI* mpTitleUI = nullptr;        // 「タイトルへ」UI
    UI* mpPauseOverlay = nullptr;   // オーバレイ
};