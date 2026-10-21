#include "TitleScene.h"
#include "InputManager.h"
#include "DxLib.h"

#include "Master.h"
#include "SceneManager.h"

TitleScene::TitleScene()
	: mTitleLogoHandle(-1)
	, mbPreviousMouseLeft(false)
	, mbStartSelected(false)
	, mTitleFloatingMotion(90, 10.0f)
{
}

void TitleScene::Initialize()
{
	// 初期値: 通常1.0倍, ホバー時1.08倍, クリック時0.94倍
	mStartScaler.Initialize(1.0f, 1.08f, 0.94f, 20.0f);

	// シーンに入った瞬間のマウス状態を記録
	mbPreviousMouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	mTitleLogoHandle = LoadGraph("Resource/Logo/TitleLogo.png");
	mStartTofuHandle = Master::mpResource->LoadGraphics("Resource/UI/Tofu.png");

	// 読み込み失敗の安全対策 (ロード失敗時は -1 が返る)
	if (mBgGroundHandle == -1)
	{
		// ログ出力やエラーハンドリング
	}
}

void TitleScene::Update(float _deltaTime)
{
	// ふわふわアニメーションの更新
	mTitleFloatingMotion.Update();

	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);

	const int startX = 500;
	const int startY = 380;
	const int startWidth = 280;
	const int startHeight = 70;

	// STARTボタンの上にマウスがあるか
	mbStartSelected =
		mouseX >= startX &&
		mouseX <= startX + startWidth &&
		mouseY >= startY &&
		mouseY <= startY + startHeight;

	// 現在マウス左クリックが押されているか
	bool currentMouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// スケーラーの更新（ホバー中か、クリック押し込み中かを渡す）
	mStartScaler.Update(mbStartSelected, currentMouseLeft, _deltaTime);

	// 「押した瞬間」だけ判定
	bool mouseLeftDown = currentMouseLeft && !mbPreviousMouseLeft;

	if (mbStartSelected && mouseLeftDown)
	{
		Master::mpSceneManager->RequestScene(
			SCENE_TYPE::STAGE_SELECT_SCENE
		);
	}

	// 前フレームの状態を保存
	mbPreviousMouseLeft = currentMouseLeft;
}

void TitleScene::Draw()
{
	// 背景
	DrawBox(
		0, 0,
		1280, 720,
		GetColor(217, 198, 143),
		TRUE
	);

	// タイトルロゴ描画
	const int baseLogoX = 270;
	const int baseLogoY = 100;
	int drawLogoY = baseLogoY + static_cast<int>(mTitleFloatingMotion.GetOffsetY());

	if (mTitleLogoHandle != -1)
	{
		DrawGraph(baseLogoX, drawLogoY, mTitleLogoHandle, TRUE);
	}
	else
	{
		DrawString(500, drawLogoY, "TITLE", GetColor(255, 255, 255));
	}

	// --- STARTボタン描画 ---
	const int startX = 500;
	const int startY = 380;
	const int startWidth = 280;
	const int startHeight = 70;

	// スケーラーから拡縮後の描画矩形を取得
	int drawLeft, drawTop, drawRight, drawBottom;
	mStartScaler.CalculateRect(startX, startY, startWidth, startHeight,
		&drawLeft, &drawTop, &drawRight, &drawBottom);

	if (mStartTofuHandle != -1)
	{
		if (mbStartSelected)
		{
			// --- ホバー時: 少し暗めの黄色 ---
			SetDrawBright(180, 180, 180);

			DrawExtendGraph(
				drawLeft,
				drawTop,
				drawRight,
				drawBottom,
				mStartTofuHandle,
				TRUE
			);

			SetDrawBright(255, 255, 255); // 元の明るさに復帰
		}
		else
		{
			// --- 通常時: 元の色そのまま ---
			DrawExtendGraph(
				drawLeft,
				drawTop,
				drawRight,
				drawBottom,
				mStartTofuHandle,
				TRUE
			);
		}
	}

	// ボタンのテキスト（ボタンの中心に合わせて追従）
	unsigned int textColor = mbStartSelected ? GetColor(180, 140, 0) : GetColor(0, 0, 0);
	int textX = (drawLeft + drawRight) / 2 - 25;
	int textY = (drawTop + drawBottom) / 2 - 10;

	DrawString(
		textX,
		textY,
		"START",
		textColor
	);
}

void TitleScene::Finalize()
{
	// 画像メモリ解放
	if (mTitleLogoHandle != -1)
	{
		DeleteGraph(mTitleLogoHandle);
		mTitleLogoHandle = -1;
	}
}