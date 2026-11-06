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
    // 操作説明をしているUIを生成する純粋関数
    UI* CreateGuideUI(const std::string _filename, const std::string _text, const VECTOR _offset);

private:
    UI* mpGuideA;
    UI* mpGuideD;
    UI* mpGuideE;
    UI* mpGuideP;
    UI* mpBackButton = nullptr;
    bool mbBackRequested = false;
};