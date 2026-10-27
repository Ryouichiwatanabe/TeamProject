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

    void SetTitleFontHandle(int _fontHandle);
    void SetMessageFontHandle(int _fontHandle);

    bool IsResumeRequested() const
    {
        return mbIsResumeRequested;
    }

    void ResetResumeRequest()
    {
        mbIsResumeRequested = false;
    }

private:
    int mTitleFontHandle = -1;
    int mMessageFontHandle = -1;

    UI* mpResumeUI = nullptr;
    UI* mpStageSelectUI = nullptr;
    UI* mpTitleUI = nullptr;

    bool mbIsResumeRequested = false;
};