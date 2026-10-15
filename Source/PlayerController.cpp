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
	// Todo : 各操作を関数化した処理を並べる
	UpdateMove();
	UpdateOperationChange();
	UpdateRotation();
	UpdatePlayerDeformationUI();
}

void PlayerController::UpdateMove()
{
	//// -- 入力値を取得 -- //
	//VECTOR inputDirection = VGet(0.0f, 0.0f, 0.0f);

	//inputDirection.x += InputManager::GetInstance().GetAxis(Axis::MoveX);
	//inputDirection.y += InputManager::GetInstance().GetAxis(Axis::MoveY);

	//if (VSize(inputDirection) == 0.0f)
	//	return;

	//// -- 入力(移動方向)をFakePlayerに渡す -- //
	//mpPlayer->Move(inputDirection);

	float rotateDirection = InputManager::GetInstance().GetAxis(Axis::Rotation);

	VECTOR inputDirection = VGet(
		rotateDirection,
		std::abs(rotateDirection / 2.0f),
		0.0f
	);

	mpPlayer->Move(inputDirection);
}

void PlayerController::UpdateOperationChange()
{
	if (InputManager::GetInstance().GetButtonDown(Button::OperationChange_toFakePlayer))
		InputManager::GetInstance().SetInputMode(InputMode::Fake);
}

void PlayerController::UpdateRotation()
{
	float rotateDirection = InputManager::GetInstance().GetAxis(Axis::Rotation);

	mpPlayer->Rotation(rotateDirection);
}

void PlayerController::UpdatePlayerDeformationUI()
{
	if (InputManager::GetInstance().GetButtonDown(Button::Shape))
		mpPlayer->ToggleDeformationUI();
}