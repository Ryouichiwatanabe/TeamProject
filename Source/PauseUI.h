#pragma once

#include "UI.h"

class PauseUI : public UI
{
public:
    PauseUI() = default;
    ~PauseUI() = default;

    void Init() override;
    void Draw() override;

    void SetTitleFontHandle(int _fontHandle);
    void SetMessageFontHandle(int _fontHandle);
    void SetButtonGraphHandle(int _graphHandle);

private:
    int mTitleFontHandle = -1;
    int mMessageFontHandle = -1;
    int mButtonGraphHandle = -1;
};