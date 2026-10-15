#pragma once
#include "Scene.h"
#include <cstddef>
#include "DxLib.h"


//---コンポジション系統
#include "FloatingMotion.h"
#include "UILabel.h"
#include "TypewriterText.h"

class Player;
class Goal;
class MovingBlock;

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
    int StageNumberFontHandle = -1;//左上のステージ番号用フォントハンドル
    int mMessageFontHandle = -1;   //下のTypeWriter用のフォントハンドル
    int mPauseBoardHandle = -1;
		
        void*  mAddedFontHandle = nullptr; // フォントハンドル

    int mCursorHandle = -1;

    void DrawCursor();

    MovingBlock* mpMovingBlock = nullptr;

private:
    int mnStageNumber = 1;

    bool mbIsPaused = false;
    bool mbPreviousP = false;
    bool mbPreviousMouseLeft = false;

    void UpdatePause();
    void DrawPause();
    void DrawClear();
    void UpdateClear();

    Player* mpPlayer = nullptr;
    bool mbIsClear = false;

    Goal* mpGoal = nullptr;

    //FontやTitleLogoなどのアニメーションをするためのコンポジション
private:

    UILabel mStageTextMotion; //Stage用
    FloatingMotion mClearTextMotion; //Clear用
    FloatingMotion mMessageMotion;//メッセージ用
    TypewriterText mTypewriter;
    bool mbIsMessageActive = false; //メッセージ表示フラグ
};
