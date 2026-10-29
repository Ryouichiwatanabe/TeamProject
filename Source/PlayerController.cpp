#include "PlayerController.h"

#include "Player.h"

#include "InputManager.h"

#include <DxLib.h>	// VECTOR用
void PlayerController::Initialize(Player* _player)
{
	mpPlayer = _player;
}

void PlayerController::Update()
{
	UpdateRotation();
}

void PlayerController::UpdateRotation()
{
	if (!mpPlayer)
		return;

	// InputManagerからの入力値を取得
	float rotateInput = InputManager::GetInstance().GetAxis(Axis::Rotation);

	// フォールバック: 直接キー入力も安全に合算
	if (CheckHitKey(KEY_INPUT_L) != 0)
	{
		rotateInput += 1.0f;
	}
	if (CheckHitKey(KEY_INPUT_J) != 0)
	{
		rotateInput -= 1.0f;
	}

	float rotateDir = 0.0f;
	if (rotateInput > 0.3f)
	{
		rotateDir = 1.0f;	// 右回転
	}
	else if (rotateInput < -0.3f)
	{
		rotateDir = -1.0f;	// 左回転
	}

	/*
	// 入力インターバル・リピート処理（一時コメントアウト）
	bool shouldRotate = false;

	// 立ち上がり検出（キー入力開始時に即時1回実行）
	if (rotateDir != 0.0f && mfPrevRotation == 0.0f)
	{
		shouldRotate = true;
		mfRepeatTimer = 0.0f;
	}
	// 長押し時のリピート処理（押しっぱなしでも滑らかに入力可能）
	else if (rotateDir != 0.0f && rotateDir == mfPrevRotation)
	{
		mfRepeatTimer += 1.0f / 60.0f;
		if (mfRepeatTimer >= 0.30f)
		{
			shouldRotate = true;
			mfRepeatTimer -= 0.20f;
		}
	}
	else
	{
		mfRepeatTimer = 0.0f;
	}

	mfPrevRotation = rotateDir;
	*/

	// インターバルなしで直接回転を呼び出し
	if (rotateDir != 0.0f)
	{
		mpPlayer->TryRotateWithShift(rotateDir);
	}
}