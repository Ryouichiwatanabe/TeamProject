#pragma once
#include "UI.h"

class OperationGuideUI : public UI
{
public:
    void Init() override;
    void Update() override;

    // 戻るボタンが押されたか（PauseUIが確認する）
    bool IsBackRequested() const { return mbBackRequested; }

private:
    UI* mpGuideA;
    UI* mpBackButton = nullptr;
    bool mbBackRequested = false;
};