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

#include "ScreenConfig.h"
#include "GameConfig.h"

// 変形状態取得用
#include "PlayerShapeUI.h"
#include "PlayerPieceUI.h"

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

	// Todo : CreateStage()内で生成 & 初期座標設定
	if (auto transform = GetModule<Transform>())
		transform->SetPosition(
			VGet(
				200.0f,
				200.0f,
				0.0f
			)
		);

	SetupPieces();
}

void Player::InitComponent()
{
	AddModule<Transform>();
	AddModule<Gravity>();
}

void Player::SetupPieces()
{
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
}

void Player::CreatePiece(int _column, int _row)
{
	// -- Pieceの生成 -- //
	auto playerPiece =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetObjectManager()
		->CreateObject<PlayerPiece>(this);
	if (playerPiece == nullptr)
		return;

	mPieces[_column][_row] = playerPiece;

	// -- ローカル座標を取得 & 設定 -- //
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
		// Todo : ここに書かずとも、GameTimeScale的なシステムを作成する
	if (_deltaTime <= 0.0f)
	{
		return;
	}

	CheackRespawn();	

	mController.Update();
	GameObject::Update(_deltaTime);
	OnPieceGrounded();
}

void Player::Draw()
{
	GameObject::Draw();
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

	float minX = 0.0f, maxX = 0.0f;
	for (const VECTOR& candidate : mCollisionCorrections) {
		if (candidate.x < minX) minX = candidate.x;
		if (candidate.x > maxX) maxX = candidate.x;
	}
	// 左右両方から一定以上押し戻されている ＝ 隙間に挟まれている！
	mbIsPinched = (minX < -1.0f && maxX > 1.0f);

	// float minX = 0.0f, maxX = 0.0f;
	float minY = 0.0f, maxY = 0.0f;
	for (const VECTOR& candidate : mCollisionCorrections)
	{
		if (candidate.x > maxX) maxX = candidate.x;
		if (candidate.x < minX) minX = candidate.x;
		if (candidate.y > maxY) maxY = candidate.y;
		if (candidate.y < minY) minY = candidate.y;
	}
	VECTOR correction = VGet(0.0f, 0.0f, 0.0f);
	// 左右両方から押されている場合（minX < 0 かつ maxX > 0）は
	// 単純に合算して相殺させる、または挟まれとして振動を抑制
	correction.x = maxX + minX;
	correction.y = maxY + minY;
	return correction;
}

void Player::ApplyCollisionCorrection()
{
	auto transform = GetModule<Transform>();
	if (transform == nullptr)
		return;

	VECTOR correction = CalculateCollisionCorrection();

	Debug::Print(
		"Player Correction: X = ",
		correction.x,
		" Y = ",
		correction.y
	);

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

void Player::OnPieceGrounded()
{
	bool isGrounded = false;

	for (auto& row : mPieces)
	{
		for (auto* piece : row)
		{
			if (piece == nullptr)
				continue;

			auto pieceGravity = piece->GetModule<Gravity>();
			bool pieceIsGrounded = pieceGravity->IsGrounded();

			if (pieceIsGrounded) isGrounded = true;
		}
	}

	auto gravity = GetModule<Gravity>();
	gravity->SetGrounded(isGrounded);
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
	// 挟まれている間は回転入力を無視する！
	if (mbIsPinched) return;

	auto transform = GetModule<Transform>();
	if (!transform) return;
	VECTOR prevRot = transform->GetRotation();
	float rotationAmount = _rotateDirection * mfRotationPower;
	float rotationRadian = rotationAmount * DX_PI_F / 180.0f;
	// 一旦回転させる
	transform->SetRotation(VAdd(prevRot, VGet(0.0f, 0.0f, rotationRadian)));
	UpdatePieceRotation();
}

void Player::ToggleDeformationUI()
{
	if (!mpShapeUI)
		return;

	mpShapeUI->ToggleEnabled();
	mpShapeUI->ApplyPieceActivityToUI();	// Pieceの有効状態をPieceUIに適用する
}

void Player::ApplyUISelectionToPlayerPiece()
{
	if (!mpShapeUI)
		return;

	for (int row = 0;
		row < GameConfig::PLAYER_PIECE_SIZE;
		++row)
	{
		for (int column = 0;
			column < GameConfig::PLAYER_PIECE_SIZE;
			++column)
		{
			auto piece = GetPlayerPiece(column, row);
			if (!piece)
				continue;

			auto pieceUI = mpShapeUI->GetPlayerPieceUI(column, row);
			if (!pieceUI)
				continue;

			piece->SetActive(pieceUI->IsSelected());
		}
	}
}

void Player::CheackRespawn()
{
	auto transform = GetModule<Transform>();
	if (transform == nullptr)
		return;

	VECTOR position = transform->GetPosition();

	// 画面下端を超えたらリスポーン
	if (position.y > ScreenConfig::SCREEN_HEIGHT)
		Respawn();;
}

void Player::Respawn()
{
	if (auto transform = GetModule<Transform>())
		transform->SetPosition(mvRespawnPos);
}