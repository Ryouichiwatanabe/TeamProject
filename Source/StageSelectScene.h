#pragma once

#include "Scene.h"
#include "StageSelectLayout.h"

class StageSelectScene : public Scene
{
public:
    void Initialize() override;
    void Update(float _deltaTime) override;
    void Draw() override;
    void Finalize() override;

private:
    // 現在選択しているステージ番号
    int mnSelectedStage = 0;

    // 前フレームのマウス左クリック状態
    bool mbPreviousMouseLeft = true;

    // ステージセレクトの背景画像
    int mBackgroundHandle = -1;

    // 各ステージのクリア画像を保持する
    // [ステージ番号 - 1]
    int mStageClearHandles[8] = {};

    // ステージ選択範囲
    // 1920×1080の背景画像上での各ステージの位置
    //
    // x     : 左端
    // y     : 上端
    // width : 横幅
    // height: 縦幅
    //
    // この範囲内をクリックすると、そのステージを選択する
    struct StageButton
    {
        float x;
        float y;
        float width;
        float height;
    };

    // 1920×1080用のステージ選択範囲
    //
    // 画像の左上から見て、
    // 上段 : Stage1 ～ Stage4
    // 下段 : Stage5 ～ Stage8
    StageButton mStageButtons[8] =
    {
        // 上段
        { 375.0f, 225.0f, 245.0f, 375.0f }, // Stage1
        { 655.0f, 225.0f, 250.0f, 375.0f }, // Stage2
        { 980.0f, 225.0f, 250.0f, 375.0f }, // Stage3
        { 1270.0f, 225.0f, 250.0f, 375.0f }, // Stage4

        // 下段
        { 375.0f, 645.0f, 245.0f, 375.0f }, // Stage5
        { 655.0f, 645.0f, 250.0f, 375.0f }, // Stage6
        { 980.0f, 645.0f, 250.0f, 375.0f }, // Stage7
        { 1270.0f, 645.0f, 250.0f, 375.0f }  // Stage8
    };

    struct StageClearImage
    {
        float x;
        float y;
        float width;
        float height;
    };

    StageClearImage mStageClearImages[8] =
    {
        // 上段
        { 375.0f, 225.0f, 245.0f, 375.0f },
        { 655.0f, 225.0f, 250.0f, 375.0f },
        { 980.0f, 225.0f, 250.0f, 375.0f },
        { 1270.0f, 225.0f, 250.0f, 375.0f },

        // 下段
        { 375.0f, 645.0f, 245.0f, 375.0f },
        { 655.0f, 645.0f, 250.0f, 375.0f },
        { 980.0f, 645.0f, 250.0f, 375.0f },
        { 1270.0f, 645.0f, 250.0f, 375.0f }
    };

    // 背景画像を1920×1080の画面いっぱいに描画する
    void DrawBackground();

    // ステージをクリアしたときの完成画像を描画する
    void DrawStageClearImages();

    // ステージ選択範囲内をクリックしたか判定する
    bool IsMouseInsideStageButton(
        int _stageIndex,
        int _mouseX,
        int _mouseY
    );

    // ステージごとの取得アイテム画像
    //
    // [ステージ番号 - 1][アイテム番号]
    //
    // 例：
    // mCollectibleHandles[0][0]
    // → Stage1のアイテム1
    //
    // mCollectibleHandles[7][2]
    // → Stage8のアイテム3
    int mCollectibleHandles[8][3] = {};

    // ステージ選択画面に取得済みアイテムを表示する
    void DrawStageCollectibleImages();
};