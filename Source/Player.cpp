#include "Player.h"

// Module関係
#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"
#include "Gravity.h"

// Piece生成用
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "PlayerPiece.h"


#include "GameConfig.h"

// 変形状態取得用
#include "PlayerShapeUI.h"

// 入力関係
#include "InputManager.h"

#include "Debug.h"

// ライブラリ
#include <DxLib.h>
#include <cmath>  //回転処理用

void Player::Init()
{
	// -- タグ設定 -- //
	SetTag(Tag::PLAYER);
	mController.Initialize(this);
}

void Player::InitComponent()
{
	// -- Module追加 -- //
	AddModule<Transform>();
}

void Player::InitPiece()
{
	// Todo : 複数Pieceに対応させる
	
	// -- piece生成 & 初期処理 -- //
	for (int row = 0;
		row < GameConfig::PLAYER_PIECE_SIZE;
		++row)
	{
		for (int column = 0;
			column < GameConfig::PLAYER_PIECE_SIZE;
			++column)
		{
			CreatePiece(column, row);
		}
	}

	/*mPieces[0][1]->SetEnabled(false);
	mPieces[0][2]->SetEnabled(false);
	mPieces[1][1]->SetEnabled(false);
	mPieces[1][2]->SetEnabled(false);*/
}

void Player::CreatePiece(int _column, int _row)
{
	auto playerPiece =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetObjectManager()
		->CreateObject<PlayerPiece>(this);

	if (playerPiece == nullptr)
		return;

	mPieces[_row][_column] = playerPiece;

	VECTOR localPosition = VGet(
		(_column - 1) * GameConfig::CELL_SIZE,
		(_row - 1) * GameConfig::CELL_SIZE,
		0.0f
	);

	playerPiece->SetLocalPosition(localPosition);
}

void Player::Update(float _deltaTime)
{
	// ポーズ中のバックグラウンドの停止
	if (_deltaTime <= 0.0f)
	{
		return;
	}
	
	if (mpShapeUI != nullptr &&
		InputManager::GetInstance().GetButtonDown(Button::Shape))
		mpShapeUI->SetEnabled(!mpShapeUI->IsEnabled());

	mController.Update();
	GameObject::Update(_deltaTime);
}

void Player::Draw()
{
	GameObject::Draw();

	auto transform = GetModule<Transform>();
	VECTOR pos = transform->GetPosition();
	Debug::Print(
		"Player座標 : (",
		pos.x,
		" , ",
		pos.y,
		" )"
	);
}

void Player::AddCollisionCorrection(VECTOR _correction)
{
	// 補正が不要なら登録しない
	if (_correction.x == 0.0f && _correction.y == 0.0f)
		return;

	mCollisionCorrections.push_back(_correction); 
}

void Player::ResetCollisionCorrections()
{
	mCollisionCorrections.clear();
}

VECTOR Player::CalculateCollisionCorrection()
{
	VECTOR correction = VGet(0.0f, 0.0f, 0.0f);

	for (const VECTOR& candidate : mCollisionCorrections)
	{
		// X軸：絶対値が最大の補正を採用
		if (fabsf(candidate.x) > fabsf(correction.x))
		{
			correction.x = candidate.x;
		}

		// Y軸：絶対値が最大の補正を採用
		if (fabsf(candidate.y) > fabsf(correction.y))
		{
			correction.y = candidate.y;
		}
	}

	return correction;
}

void Player::ApplyCollisionCorrection()
{
	auto transform = GetModule<Transform>();
	if (transform == nullptr)
		return;

	VECTOR correction = CalculateCollisionCorrection();

	VECTOR position = transform->GetPosition();
	position = VAdd(position, correction);

	transform->SetPosition(position);
}

void Player::BeginCollisionResolution()
{
	GameObject::BeginCollisionResolution();	// memo : 意味ないけど一応

	ResetCollisionCorrections();
}

void Player::EndCollisionResolution()
{
	GameObject::EndCollisionResolution();	// memo : 意味ないけど一応

	ApplyCollisionCorrection();

	UpdatePiecePosition();
	UpdatePieceRotation();
}

void Player::UpdatePiecePosition()
{
	for (auto& row : mPieces)
	{
		for (auto* piece : row)
		{
			if (piece == nullptr)
				continue;

			piece->UpdateWorldPosition();
		}
	}
}

void Player::UpdatePieceRotation()
{
	for (auto& row : mPieces)
	{
		for (auto* piece : row)
		{
			if (piece == nullptr)
				continue;

			piece->UpdateRotation();
		}
	}
}

void Player::Move(VECTOR _direction)
{
	if (VSize(_direction) == 0.0f)
		return;

	// -- 移動量を取得 & 座標反映 -- //
	VECTOR moveAmount = VScale(_direction, mfSpeed);

	auto transform = GetModule<Transform>();
	VECTOR nextPos = VAdd(
		transform->GetPosition(),
		moveAmount
	);

	transform->SetPosition(nextPos);

	UpdatePiecePosition();
}

void Player::Rotation(float _rotateDirection)
{
	// -- 回転値を取得 -- //
	float rotationAmount = _rotateDirection * mfRotationPower;	// 回転の値(度数)
	float rotationRadian = rotationAmount * DX_PI_F / 180.0f;	// 回転の値(ラジアン)

	// -- 回転値を適用 -- //
	if (auto transform = GetModule<Transform>())
		transform->SetRotation(
			VAdd(
				transform->GetRotation(),
				VGet(0.0f, 0.0f, rotationRadian)
			)
		);

	UpdatePieceRotation();
}

void Player::ApplyShapeFromUI()
{
	for (int row = 0;
		row < GameConfig::PLAYER_PIECE_SIZE;
		++row)
	{
		for (int column = 0;
			column < GameConfig::PLAYER_PIECE_SIZE;
			++column)
		{
			bool selected = mpShapeUI->IsPieceSelected(column, row);
			mPieces[column][row]->SetEnabled(selected);
		}
	}
}

void Player::Respawn()
{
	GetModule<Transform>()->SetPosition(mvRespawnPos);
}

bool Player::IsPieceEnabled(int _column, int _row)
{
	return mPieces[_column][_row]->GetEnabled();
}
