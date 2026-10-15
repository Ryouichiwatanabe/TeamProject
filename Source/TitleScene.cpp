#include "TitleScene.h"
#include "InputManager.h"
#include "DxLib.h"

#include "Master.h"
#include "SceneManager.h"

TitleScene::TitleScene()
	:mTitleLogoHandle(-1)
	,mbPreviousMouseLeft(false)
	,mbStartSelected(false)
	,mTitleFloatingMotion(90,10.0f)
{
}

void TitleScene::Initialize()
{
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
	//==ふわふわアニメーションの更新==//
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
	bool currentMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// 「押した瞬間」だけ判定
	bool mouseLeftDown =
		currentMouseLeft && !mbPreviousMouseLeft;

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

	const int baseLogoX = 270;
	const int baseLogoY = 100;
	// FloatingMotionから取得したYオフセットを基準Y座標に加算
	int drawLogoY = baseLogoY + static_cast<int>(mTitleFloatingMotion.GetOffsetY());

	if (mTitleLogoHandle != -1)
	{
		DrawGraph(baseLogoX, drawLogoY, mTitleLogoHandle, TRUE);
	}
	else
	{
		DrawString(500, drawLogoY, "TITLE", GetColor(255, 255, 255));
	}

	// タイトル
	if (mTitleLogoHandle != -1)
	{
		DrawGraph(270, 100, mTitleLogoHandle, TRUE);
	}
	else 
	{
		DrawString(500, 250, "TITLE", GetColor(255, 255, 255));
	}




	/*DrawString(
		500,
		250,
		"AAAAAA",
		GetColor(0, 0, 0)
	);*/

	// スタート
	const int startX = 500;
	const int startY = 380;
	const int startWidth = 280;
	const int startHeight = 70;

	if (mStartTofuHandle != -1)
	{
		DrawExtendGraph(
			startX,
			startY,
			startX + startWidth,
			startY + startHeight,
			mStartTofuHandle,
			TRUE
		);
	}

	DrawString(
		startX + 115,
		startY + 25,
		"START",
		GetColor(0, 0, 0)
	);
}

void TitleScene::Finalize()
{
	//==画像メモリ解放==//
	if (mTitleLogoHandle != -1)
	{
		DeleteGraph(mTitleLogoHandle);
		mTitleLogoHandle = -1;

	}

	// 終了処理
}