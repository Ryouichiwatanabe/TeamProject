#include "PauseUI.h"

void PauseUI::Init()
{
    SetEnabled(false);
}

void PauseUI::SetTitleFontHandle(int _fontHandle)
{
    mTitleFontHandle = _fontHandle;
}

void PauseUI::SetMessageFontHandle(int _fontHandle)
{
    mMessageFontHandle = _fontHandle;
}

void PauseUI::SetButtonGraphHandle(int _graphHandle)
{
    mButtonGraphHandle = _graphHandle;
}

void PauseUI::Draw()
{
    // 暗転
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);

    DrawBox(
        0,
        0,
        1280,
        720,
        GetColor(0, 0, 0),
        TRUE
    );

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0
    );

    // Pauseタイトル
    DrawStringToHandle(
        570,
        200,
        "Pause",
        GetColor(255, 255, 255),
        mTitleFontHandle
    );

    const int buttonWidth = 280;
    const int buttonHeight = 70;

    // 再開ボタン
    if (mButtonGraphHandle != -1)
    {
        DrawExtendGraph(
            500,
            300,
            500 + buttonWidth,
            300 + buttonHeight,
            mButtonGraphHandle,
            TRUE
        );
    }

    DrawStringToHandle(
        605,
        325,
        "再開",
        GetColor(0, 0, 0),
        mMessageFontHandle
    );

    // ステージ選択
    if (mButtonGraphHandle != -1)
    {
        DrawExtendGraph(
            500,
            400,
            500 + buttonWidth,
            400 + buttonHeight,
            mButtonGraphHandle,
            TRUE
        );
    }

    DrawStringToHandle(
        505,
        425,
        "ステージ選択に戻る",
        GetColor(0, 0, 0),
        mMessageFontHandle
    );

    // タイトルへ戻る
    if (mButtonGraphHandle != -1)
    {
        DrawExtendGraph(
            500,
            500,
            500 + buttonWidth,
            500 + buttonHeight,
            mButtonGraphHandle,
            TRUE
        );
    }

    DrawStringToHandle(
        540,
        525,
        "タイトルへ戻る",
        GetColor(0, 0, 0),
        mMessageFontHandle
    );
}