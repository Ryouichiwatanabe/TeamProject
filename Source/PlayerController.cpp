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

	float rotateInput = InputManager::GetInstance().GetAxis(Axis::Rotation);

	// フォールバック: 直接キー入力も安全に合算
	if (CheckHitKey(KEY_INPUT_D) != 0)
	{
		rotateInput += 1.0f;
	}
	if (CheckHitKey(KEY_INPUT_A) != 0)
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

	// インターバルなしで直接回転を呼び出し
	if (rotateDir != 0.0f)
	{
		mpPlayer->TryRotateWithShift(rotateDir);
	}
}