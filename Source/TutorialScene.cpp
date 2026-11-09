#include "TutorialScene.h"

// -- Manager -- //
#include "Master.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "UIManager.h"
#include "EffectManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "ScreenConfig.h"

// -- GameObject / UI / Effect -- //
#include "Stage.h"
#include "Player.h"
#include "Goal.h"
#include "UIBackground.h"
#include "PlayerShapeUI.h"
#include "PauseUI.h"
#include "EffectClear.h"
#include "UI.h"

#include "StageClearData.h" // SetTutorialCleared()

#include <DxLib.h>
#include <string>

void TutorialScene::Initialize()
{
    mbIsClear = false;
    mfClearTimer = 0.0f;

    // -- Player・変形UIの生成 (変形UIはInit内でPlayerを探すため後に作る) -- //
    mpPlayer = GetObjectManager()->CreateObject<Player>();
    if (mpPlayer != nullptr)
    {
        mpShapeUI = GetUIManager()->CreateUI<PlayerShapeUI>();
    }

    // -- ステージ読み込み -- //
    auto* stage = new Stage();
    stage->Load("Resource/Stage/Stage0.csv");

    mpGoal = GetObjectManager()->FindObject<Goal>();

    // -- 背景UI -- //
    mpBackgroundUI = GetUIManager()->CreateUI<UIBackground>();
    if (mpBackgroundUI != nullptr)
    {
        mpBackgroundUI->SetStageByNumber(0);
    }

    // -- ポーズUI -- //
    mpPauseUI = GetUIManager()->CreateUI<PauseUI>();
    if (mpPauseUI != nullptr)
    {
        mpPauseUI->SetEnabled(false);
        mpPauseUI->SetResumeOnly(true);
    }

    // -- 「Skip」ボタン -- //
    mpSkipButton = GetUIManager()->CreateUI<UI>();
    mpSkipButton->LoadUIGraph("Resource/UI/Scene_Title/Spoon.png");
    mpSkipButton->SetEnabled(true);
    mpSkipButton->SetLayer(10);
    mpSkipButton->GetHoverScaler().SetActive(true);
    mpSkipButton->GetText().SetFontId(FontId::SubTitle);
    mpSkipButton->GetText().SetColor(GetColor(255, 255, 255));
    mpSkipButton->GetText().SetText("Skip");
    mpSkipButton->GetTransform().SetPosition(VGet(480.0f, -280.0f, 0.0f));

    // -- メッセージ表示設定 -- //
    mMessageFontHandle = CreateFontToHandle("PixelMplus10", 36, -1, DX_FONTTYPE_EDGE);
    mnTutorialStep = 0;
    mfStepTimer = 0.0f; // ★タイマーを0に初期化
    mpMessageUI = GetUIManager()->CreateUI<UI>();
    mpMessageUI->SetEnabled(false);
    mTypewriter.Initialize(mpMessageUI);
    mTypewriter.SetFrameInterval(2);
    // ★最初は導入メッセージを表示！
    SetTutorialMessage("これからチュートリアルを始めるよ！\n基本の操作を一緒に覚えよう！");

    // -- BGM再生 -- //
    SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Game, true);
}

void TutorialScene::SetTutorialMessage(const char* _text)
{
    if (mpMessageUI == nullptr)
        return;

    mpMessageUI->GetText().SetText(_text);
    mTypewriter.Start();
}

void TutorialScene::Update(float _deltaTime)
{
    // -- クリア演出中の更新 -- //
    if (mbIsClear)
    {
        mTypewriter.Update();
        if (GetEffectManager() != nullptr)
        {
            GetEffectManager()->Update(_deltaTime);
        }

        mfClearTimer += _deltaTime;

        if (mnTutorialStep != 99 && mfClearTimer >= 1.8f)
        {
            mnTutorialStep = 99;
            SetTutorialMessage("ここからは材料があるから、\nそれを拾って豆腐料理を作ろう！");
        }

        if (mfClearTimer >= 5.0f ||
            (mnTutorialStep == 99 && mTypewriter.IsFinished() && InputManager::GetInstance().GetButtonDown(Button::Confirm)))
        {
            StageClearData::SetTutorialCleared(true);   // チュートリアルのクリアを記録
            Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
        }
        return;
    }

    // -- 「Skip」ボタンが押された場合 -- //
    if (mpSkipButton != nullptr && mpSkipButton->GetButton().IsClicked())
    {
        InputManager::GetInstance().ConsumeButton(Button::Confirm);
        SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);

        StageClearData::SetTutorialCleared(true);   // チュートリアルのクリアを記録
        Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
        return;
    }

 
    // -- ポーズ制御 & 最初の挨拶中（Step 0）の操作ロック -- //
    if (mnTutorialStep > 0)
    {
        // Step 0 が終わってからポーズキーを受け付ける
        UpdatePause();
    }

    float gameDeltaTime = _deltaTime;

    // ポーズ中、または「これからチュートリアルを始めるよ（Step 0）」の間は時間を止めて操作不可にする
    if ((mpPauseUI != nullptr && mpPauseUI->IsEnabled()) || mnTutorialStep == 0)
    {
        gameDeltaTime = 0.0f;
    }
    else
    {
        // Step 0 が終わっており、かつポーズ中でないときだけ変形UIの開閉を受け付ける
        UpdateShapeUIToggle();
    }


    // -- メッセージ・進行ステップ更新 -- //
    mTypewriter.Update();
    UpdateTutorialStep(_deltaTime);

    Scene::Update(gameDeltaTime);

    // -- ゴール判定 -- //
    if (mpGoal != nullptr && mpGoal->IsPlayerTouching())
    {
        mbIsClear = true;
        mfClearTimer = 0.0f;

        if (mpPauseUI != nullptr) mpPauseUI->SetEnabled(false);
        if (mpShapeUI != nullptr && mpShapeUI->IsEnabled()) mpShapeUI->Close();

        SoundManager::GetInstance()->StopBGM();
        SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_GOALVOICE);

        if (GetEffectManager() != nullptr)
        {
            GetEffectManager()->Play<EffectClear>(
                ScreenConfig::SCREEN_WIDTH / 2.0f,
                ScreenConfig::SCREEN_HEIGHT / 2.0f
            );
        }

        SetTutorialMessage("チュートリアルクリア！ おめでとう！");
    }
}

void TutorialScene::UpdateTutorialStep(float _deltaTime)
{
    auto& input = InputManager::GetInstance();

    // Step 0: 最初の挨拶文を読む時間（文字が出終わってから2.0秒経ったら回転の説明へ）
    if (mnTutorialStep == 0)
    {
        if (mTypewriter.IsFinished())
        {
            mfStepTimer += _deltaTime;
            if (mfStepTimer >= 2.0f) // ★読む時間の長さ（2.0秒）
            {
                mnTutorialStep = 1;
                SetTutorialMessage("まずは [J][L] キーを押して、\n豆腐を回転させてみよう！");
            }
        }
    }
    // Step 1: 回転入力で変形説明へ
    else if (mnTutorialStep == 1)
    {
        if (input.GetAxis(Axis::Rotation) != 0.0f ||
            CheckHitKey(KEY_INPUT_J) != 0 || CheckHitKey(KEY_INPUT_L) != 0)
        {
            mnTutorialStep = 2;
            SetTutorialMessage("ナイス！ 次は変形キーを押して、\n豆腐の形を変えてみよう！");
        }
    }
    // Step 2: 変形UIを開いたらポーズ説明へ
    else if (mnTutorialStep == 2)
    {
        if (mpShapeUI != nullptr && mpShapeUI->IsEnabled())
        {
            mnTutorialStep = 3;
            SetTutorialMessage("マスをクリックして変形できるよ！\n次は [P]キー でポーズ画面を開いてみよう！");
        }
    }
    // Step 3: ポーズ画面を開いたら再開の案内へ
    else if (mnTutorialStep == 3)
    {
        if (mpPauseUI != nullptr && mpPauseUI->IsEnabled())
        {
            mnTutorialStep = 4;
            SetTutorialMessage("ポーズ画面では再開やステージ選択へ戻れるよ！\n「再開」を押そう！");
        }
    }
    // Step 4: ポーズ画面を閉じたらゴール案内へ
    else if (mnTutorialStep == 4)
    {
        if (mpPauseUI != nullptr && !mpPauseUI->IsEnabled())
        {
            mnTutorialStep = 5;
            SetTutorialMessage("バッチリ！\nそれではステージ右にあるゴールを目指そう！");
        }
    }
}

void TutorialScene::UpdatePause()
{
    if (mpPauseUI == nullptr)
        return;

    if (InputManager::GetInstance().GetButtonDown(Button::Pause))
    {
        mpPauseUI->ToggleEnabled();
    }
}

void TutorialScene::UpdateShapeUIToggle()
{
    if (mpShapeUI == nullptr)
        return;

    if (!InputManager::GetInstance().GetButtonDown(Button::Shape))
        return;

    if (mpShapeUI->IsEnabled())
        mpShapeUI->Close();
    else
        mpShapeUI->Open();
}

void TutorialScene::Draw()
{
    // -- 背景描画 -- //
    if (mpBackgroundUI != nullptr)
    {
        mpBackgroundUI->DrawBackground();
    }

    Scene::Draw();

    // -- メッセージウィンドウ描画 (左上に大きく配置) -- //
    constexpr int boxLeft = 30;
    constexpr int boxTop = 25;
    constexpr int boxRight = 920;  // ★大きな文字が余裕で収まるように横幅を拡大
    constexpr int boxBottom = 165;  // ★縦幅も140pxに拡大

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 190);
    DrawBox(boxLeft, boxTop, boxRight, boxBottom, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 外枠を2重線にしてはっきり見やすくする
    DrawBox(boxLeft, boxTop, boxRight, boxBottom, GetColor(255, 255, 255), FALSE);
    DrawBox(boxLeft + 2, boxTop + 2, boxRight - 2, boxBottom - 2, GetColor(255, 255, 255), FALSE);

    // -- テキストをウィンドウ内に大きく描画（改行対応） -- //
    if (mMessageFontHandle != -1 && mpMessageUI != nullptr)
    {
        const std::string visibleText = mTypewriter.GetVisibleText();

        constexpr int textX = boxLeft + 30; // 左端から30px内側
        constexpr int textY = boxTop + 25;  // 上端から25px内側
        constexpr int lineSpacing = 48;     // ★36pxフォントに合わせて行間も広く(48px)

        size_t newlinePos = visibleText.find('\n');
        if (newlinePos == std::string::npos)
        {
            DrawStringToHandle(textX, textY, visibleText.c_str(), GetColor(255, 255, 255), mMessageFontHandle);
        }
        else
        {
            std::string line1 = visibleText.substr(0, newlinePos);
            std::string line2 = visibleText.substr(newlinePos + 1);

            DrawStringToHandle(textX, textY, line1.c_str(), GetColor(255, 255, 255), mMessageFontHandle);
            DrawStringToHandle(textX, textY + lineSpacing, line2.c_str(), GetColor(255, 255, 255), mMessageFontHandle);
        }
    }
}

void TutorialScene::Finalize()
{
    if (mMessageFontHandle != -1)
    {
        DeleteFontToHandle(mMessageFontHandle);
        mMessageFontHandle = -1;
    }

    Scene::Finalize();
}