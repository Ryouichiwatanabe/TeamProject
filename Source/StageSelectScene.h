#pragma once

#include "Scene.h"

class StageSelectScene : public Scene
{
public:
    void Initialize() override;
    void Update(float _deltaTime) override;
    void Draw() override;
    void Finalize() override;

private:
    // 現在選択しているステージ
    int mnSelectedStage = 0;

    // 前フレームのマウス左クリック状態
    bool mbPreviousMouseLeft = true;

    // ステージセレクト背景画像
    int mBackgroundHandle = -1;
    void DrawStageClearImages();
    int mStageClearHandles[8] = {};
    int mStage1ClearHandle = -1;

    // 画像上のステージ選択位置
    struct StageButton
    {
        float x;
        float y;
        float width;
        float height;
    };

    StageButton mStageButtons[8] =
    {
        // 上段
        { 189.0f, 166.5f, 110.0f, 80.0f },
        { 331.5f, 166.5f, 110.0f, 80.0f },
        { 493.0f, 166.5f, 110.0f, 80.0f },
        { 637.7f, 166.5f, 110.0f, 80.0f },

        // 下段
        { 189.0f, 370.0f, 110.0f, 80.0f },
        { 337.5f, 370.0f, 110.0f, 80.0f },
        { 497.5f, 370.0f, 110.0f, 80.0f },
        { 634.7f, 370.0f, 110.0f, 80.0f }
    };

    // 背景画像を画面いっぱいに描画
    void DrawBackground();

    /*void DrawStageButtonDebug();*/

    // ステージボタンのクリック判定
    bool IsMouseInsideStageButton(
        int _stageIndex,
        int _mouseX,
        int _mouseY
    );
};