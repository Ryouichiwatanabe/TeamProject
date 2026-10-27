#pragma once

#include "Scene.h"
#include <cstddef>


//---コンポジション系統
#include "FloatingMotion.h"
#include "UILabel.h"
#include "UITypewriterText.h"

class Player;
class Goal;
class MovingBlock;
class CollectibleIconUI;
class PauseUI;
class ClearUI;


//class Player_test;

class GameScene : public Scene
{
public:
    void Initialize();
    void Update(float deltaTime);
    void Draw();
    void Finalize();
    void SetStageNumber(int _stageNumber);

private:
    int StageNumberFontHandle = -1; // 左上のステージ番号用フォントハンドル
    int mMessageFontHandle = -1;    // 下のTypeWriter用のフォントハンドル

    void* mAddedFontHandle = nullptr; // フォントハンドル

    int mCursorHandle = -1;

    void DrawCursor();

    MovingBlock* mpMovingBlock = nullptr;

    PauseUI* mpPauseUI = nullptr;
    ClearUI* mpClearUI = nullptr;

private:
    int mnStageNumber = 1;

    bool mbIsPaused = false;
    bool mbPreviousP = false;
    bool mbPreviousMouseLeft = false;

    void UpdatePause();
    void UpdateClear();

    Player* mpPlayer = nullptr;
    bool mbIsClear = false;

    Goal* mpGoal = nullptr;

    CollectibleIconUI* mpCollectibleIconUI[3] = {};

    bool mbIsCollectibleCollected[3] = {};

    // FontやTitleLogoなどのアニメーションをするためのコンポジション
private:

    UILabel mStageTextMotion;          // Stage用
    FloatingMotion mClearTextMotion;   // Clear用
    FloatingMotion mMessageMotion;     // メッセージ用
    float mfClearItemTimer = 0.0f;

    bool mbIsMessageActive = false;   // メッセージ表示フラグ
};