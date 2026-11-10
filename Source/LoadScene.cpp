#include "LoadScene.h"
#include "Master.h"
#include "SceneManager.h"
#include "FontManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "ScreenConfig.h"
#include <DxLib.h>
#include <cmath>

void LoadScene::Initialize()
{
    mfTimer = 0.0f;
    mbReady = false;

    // 視認性抜群の40pxフォントを作成（文字が潰れずハッキリ読める設定）
    mnCustomFont = CreateFontToHandle("PixelMplus10", 40, -1, DX_FONTTYPE_ANTIALIASING);

    // 選択されたステージ番号を取得
    mnStageNumber = Master::mpSceneManager->GetStageNumber();

    // ステージに応じた自然な日本語のTipsとお料理メモ
    switch (mnStageNumber)
    {
    case 1:
        mTipTitle = "【 お料理メモ：変形 】";
        mTipContent = "「Eキー」を押すと変形できるよ！ 状況に合わせて豆腐の姿を変えよう。";
        break;
    case 2:
        mTipTitle = "【 お料理メモ：回転移動 】";
        mTipContent = "「Aキー / Dキー」で左右にコロコロ転がって移動しよう！";
        break;
    case 3:
        mTipTitle = "【 ギミック：スイッチブロック 】";
        mTipContent = "スイッチを踏むと、同じ色のブロックが現れたり消えたりするよ。";
        break;
    case 4:
        mTipTitle = "【 ギミック：トゲトゲ 】";
        mTipContent = "赤いトゲにぶつかると豆腐が崩れちゃう！ 上手に飛び越えよう。";
        break;
    case 5:
        mTipTitle = "【 ギミック：バターブロック 】";
        mTipContent = "黄色いバターの上はツルツル滑るよ！ 滑る勢いを使って進もう。";
        break;
    case 6:
        mTipTitle = "【 ギミック：崩れる足場 】";
        mTipContent = "ヒビ割れたブロックは乗ると崩れるよ！ 立ち止まらず素早く渡ろう。";
        break;
    case 7:
        mTipTitle = "【 攻略のコツ：形の使い分け 】";
        mTipContent = "高い段差は「たて長」、低い天井のすき間は「よこ長」が通りやすいよ。";
        break;
    case 8:
        mTipTitle = "【 やりこみ：調味料あつめ 】";
        mTipContent = "ステージの調味料を3つ集めると、レシピ帳に料理が完成するよ！";
        break;
    default:
        mTipTitle = "【 攻略のヒント 】";
        mTipContent = "「Eキー」で変形！ 困ったときはリスタートボタンを長押ししよう。";
        break;
    }
}

void LoadScene::Update(float _deltaTime)
{
    Scene::Update(_deltaTime);

    mfTimer += _deltaTime;

    // 100%までゲージが溜まったら準備完了
    if (mfTimer >= LOAD_DURATION)
    {
        mbReady = true;

        // 準備完了後、マウスクリック（左クリック）または 決定キー(Enter/Space)でゲームへ
        const bool isClicked = ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) ||
            InputManager::GetInstance().GetButtonDown(Button::Confirm);

        if (isClicked)
        {
            // 決定音を鳴らしてゲーム画面へ遷移
            SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);
            Master::mpSceneManager->RequestScene(SCENE_TYPE::GAME_SCENE);
        }
    }
}

void LoadScene::Draw()
{
    const int sw = static_cast<int>(ScreenConfig::SCREEN_WIDTH);
    const int sh = static_cast<int>(ScreenConfig::SCREEN_HEIGHT);
    const int cx = sw / 2; // 画面中央X

    // 1. 背景：レシピ本のような温かみのあるモカブラウン
    DrawBox(0, 0, sw, sh, GetColor(55, 40, 30), TRUE);

    // 上下の飾りライン（出汁・きな粉カラー）
    DrawBox(60, 40, sw - 60, 44, GetColor(230, 215, 185), TRUE);
    DrawBox(60, sh - 44, sw - 60, sh - 40, GetColor(230, 215, 185), TRUE);

    int font = (mnCustomFont != -1) ? mnCustomFont : (Master::mpFontManager ? Master::mpFontManager->GetHandle(FontId::HUD) : -1);

    // -------------------------------------------------------------
    // 2. 中央のしたごしらえパネル（レシピノート風）
    // -------------------------------------------------------------
    const int cardW = 960;
    const int cardH = 430;
    const int cardTop = 160;
    const int cardBottom = cardTop + cardH;
    const int cardLeft = cx - cardW / 2;
    const int cardRight = cx + cardW / 2;

    // パネルの影とアイボリーのノート用紙
    DrawRoundRect(cardLeft + 8, cardTop + 8, cardRight + 8, cardBottom + 8, 20, 20, GetColor(30, 20, 15), TRUE);
    DrawRoundRect(cardLeft, cardTop, cardRight, cardBottom, 20, 20, GetColor(250, 246, 235), TRUE);
    DrawRoundRect(cardLeft, cardTop, cardRight, cardBottom, 20, 20, GetColor(85, 65, 50), FALSE);

    // 「ステージ X」
    std::string stageText = "ステージ " + std::to_string(mnStageNumber);
    if (font != -1)
    {
        int textW = GetDrawStringWidthToHandle(stageText.c_str(), static_cast<int>(stageText.length()), font);
        DrawStringToHandle(cx - textW / 2, cardTop + 35, stageText.c_str(), GetColor(185, 65, 45), font);
    }

    // -------------------------------------------------------------
    // 3. 料理モチーフ演出：お鍋のコトコト湯気（ゆげ）アニメーション
    // -------------------------------------------------------------
    const int steamCenterY = cardTop + 165;

    // お鍋のコトコト温かい湯気がゆらゆら昇っていく演出
    for (int i = 0; i < 3; ++i)
    {
        float t = std::fmod(mfTimer * 1.6f + i * 0.33f, 1.0f);
        float steamY = steamCenterY - t * 50.0f;
        float steamX = cx + (i - 1) * 38.0f + std::sin(t * 6.28f + i) * 12.0f;
        int alpha = static_cast<int>(std::sin(t * 3.14159f) * 170);

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
        DrawCircleAA(steamX, steamY, 14.0f * (0.8f + t * 0.5f), 16, GetColor(210, 190, 170), TRUE);
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 進行率（0%?100%）
    float progress = (std::min)(1.0f, mfTimer / LOAD_DURATION);
    int percent = static_cast<int>(progress * 100.0f);

    // 状態テキスト（したごしらえ中... / したごしらえ完了！）
    if (!mbReady)
    {
        std::string statusText = "したごしらえ中... " + std::to_string(percent) + "%";
        if (font != -1)
        {
            int textW = GetDrawStringWidthToHandle(statusText.c_str(), static_cast<int>(statusText.length()), font);
            DrawStringToHandle(cx - textW / 2, cardTop + 220, statusText.c_str(), GetColor(70, 50, 40), font);
        }
    }
    else
    {
        std::string readyText = "したごしらえ完了！";
        if (font != -1)
        {
            int textW = GetDrawStringWidthToHandle(readyText.c_str(), static_cast<int>(readyText.length()), font);
            DrawStringToHandle(cx - textW / 2, cardTop + 220, readyText.c_str(), GetColor(45, 135, 55), font);
        }

        // ふわふわ点滅する「画面をクリックしてスタート」
        const float blink = (std::sin(mfTimer * 6.0f) + 1.0f) * 0.5f;
        int alpha = static_cast<int>(120 + 135 * blink);

        std::string clickText = ">>  画面をクリックしてスタート  <<";
        if (font != -1)
        {
            int textW = GetDrawStringWidthToHandle(clickText.c_str(), static_cast<int>(clickText.length()), font);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
            DrawStringToHandle(cx - textW / 2, cardTop + 280, clickText.c_str(), GetColor(190, 55, 35), font);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }

    // -------------------------------------------------------------
    // 4. お出汁のプログレスバー（きな粉・お醤油色）
    // -------------------------------------------------------------
    const int barW = 760;
    const int barH = 22;
    const int barX = cx - barW / 2;
    const int barY = cardBottom - 60;

    // ゲージの背景（薄いベージュ）
    DrawRoundRect(barX, barY, barX + barW, barY + barH, 11, 11, GetColor(220, 210, 195), TRUE);
    // ゲージの進行バー（おいしそうな出汁オレンジ）
    if (percent > 0)
    {
        int fillW = static_cast<int>(barW * progress);
        DrawRoundRect(barX, barY, barX + fillW, barY + barH, 11, 11, GetColor(230, 140, 50), TRUE);
    }
    DrawRoundRect(barX, barY, barX + barW, barY + barH, 11, 11, GetColor(95, 75, 60), FALSE);

    // -------------------------------------------------------------
    // 5. 下部のお料理メモ（文字がハッキリ読めるワイドプレート）
    // -------------------------------------------------------------
    const int tipBoxW = 1300;
    const int tipBoxH = 170;
    const int tipTop = sh - 250;
    const int tipBottom = tipTop + tipBoxH;
    const int tipLeft = cx - tipBoxW / 2;
    const int tipRight = cx + tipBoxW / 2;

    // 濃いこげ茶のプレート（文字とのコントラストを最大にして視認性を確保）
    DrawRoundRect(tipLeft + 6, tipTop + 6, tipRight + 6, tipBottom + 6, 18, 18, GetColor(25, 18, 12), TRUE);
    DrawRoundRect(tipLeft, tipTop, tipRight, tipBottom, 18, 18, GetColor(45, 32, 24), TRUE);
    DrawRoundRect(tipLeft, tipTop, tipRight, tipBottom, 18, 18, GetColor(215, 190, 150), FALSE);

    // 見出し（あたたかい山吹色）
    if (font != -1)
    {
        int titleW = GetDrawStringWidthToHandle(mTipTitle.c_str(), static_cast<int>(mTipTitle.length()), font);
        DrawStringToHandle(cx - titleW / 2, tipTop + 24, mTipTitle.c_str(), GetColor(255, 210, 85), font);
    }

    // 本文（純白のクッキリ文字・中央揃え）
    if (font != -1)
    {
        int contentW = GetDrawStringWidthToHandle(mTipContent.c_str(), static_cast<int>(mTipContent.length()), font);
        DrawStringToHandle(cx - contentW / 2, tipTop + 90, mTipContent.c_str(), GetColor(255, 255, 255), font);
    }

    Scene::Draw();
}

void LoadScene::Finalize()
{
    // 自前で作成したフォントハンドルを解放
    if (mnCustomFont != -1)
    {
        DeleteFontToHandle(mnCustomFont);
        mnCustomFont = -1;
    }

    Scene::Finalize();
}