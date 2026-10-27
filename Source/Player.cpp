#include "Player.h"

//SE・BGM
#include "SoundManager.h"

// Module関係
#include "Transform.h"
#include "Gravity.h"

// -- Piece生成用 -- //
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "PlayerPiece.h"

#include "ScreenConfig.h"
#include "GameConfig.h"
#include "PlayerMath.h"
#include "Debug.h"

#include <DxLib.h>
#include <cmath>

// ============================================================ //
//  生成
// ============================================================ //
void Player::Init()
{
	SetTag(Tag::PLAYER);

	mController.Initialize(this);

	SetupPieces();
	SetShape(0);	// 初期形状 : T字

	UpdatePieceTransforms();
}

void Player::InitComponent()
{
	AddModule<Transform>();
	AddModule<Gravity>();
}

void Player::SetupPieces()
{
	for (int row = 0; row < GameConfig::PLAYER_PIECE_SIZE; ++row)
	{
		for (int column = 0; column < GameConfig::PLAYER_PIECE_SIZE; ++column)
		{
			CreatePiece(column, row);
		}
	}
}

void Player::CreatePiece(int _column, int _row)
{
	auto scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
	auto objectManager = scene ? scene->GetObjectManager() : nullptr;
	if (!objectManager)
		return;

	auto playerPiece = objectManager->CreateObject<PlayerPiece>(this);
	if (!playerPiece)
		return;

	mPieces[_column][_row] = playerPiece;

	// 仮の相対座標 : 中央マス(1,1)を原点とする
	// (SetShape → RecalculateRotationPivot で回転軸Piece基準に置き直される)
	playerPiece->SetLocalPosition(VGet(
		static_cast<float>(_column - 1) * GameConfig::CELL_SIZE,
		static_cast<float>(_row - 1) * GameConfig::CELL_SIZE,
		0.0f
	));
}

// ============================================================ //
//  毎フレームの更新
// ============================================================ //
void Player::Update(float _deltaTime)
{
	if (_deltaTime <= 0.0f)
		return;

	// 変形UIを開いている間は停止(基盤ブロックを動かさないため)
	if (mbIsShapeEditing)
	{
		UpdatePieceTransforms();
		Debug::Print("Shape Editing...");
		return;
	}

	if (mfInputCooldown > 0.0f)
		mfInputCooldown -= _deltaTime;

	CheckRespawn();

	mController.Update();			// 入力 → 回転
	GameObject::Update(_deltaTime);	// 重力などのModule
	UpdatePieceTransforms();

	PrintDebugInfo();
}

void Player::Draw()
{
	GameObject::Draw();
}

void Player::PrintDebugInfo()
{
	auto transform = GetTransform();
	if (!transform)
		return;

	int deg = static_cast<int>(std::roundf(transform->GetRotation().z * 180.0f / DX_PI_F)) % 360;
	if (deg < 0)
		deg += 360;

	Debug::Print("Player Pos: (", static_cast<int>(transform->GetPosition().x), ", ", static_cast<int>(transform->GetPosition().y), ")");
	Debug::Print("Player Angle: ", deg, " deg");
	Debug::Print("Grounded: ", mbIsGrounded ? "YES" : "NO", " / Toppling: ", mbIsToppling ? "YES" : "NO");
	Debug::Print("Input Cooldown: ", mfInputCooldown > 0.0f ? "ACTIVE" : "IDLE");
}

// ============================================================ //
//  Pieceの座標同期・重心
// ============================================================ //
void Player::UpdatePieceTransforms()
{
	auto transform = GetTransform();
	if (!transform)
		return;

	const VECTOR playerPos = transform->GetPosition();
	const float rotZ = transform->GetRotation().z;

	for (int r = 0; r < GameConfig::PLAYER_PIECE_SIZE; ++r)
	{
		for (int c = 0; c < GameConfig::PLAYER_PIECE_SIZE; ++c)
		{
			auto piece = mPieces[c][r];
			if (!piece || !piece->IsActive())
				continue;

			auto pieceTransform = piece->GetModule<Transform>();
			if (!pieceTransform)
				continue;

			pieceTransform->SetPosition(
				PlayerMath::LocalToWorld(playerPos, rotZ, piece->GetLocalPosition()));
			pieceTransform->SetRotation(VGet(0.0f, 0.0f, rotZ));
		}
	}
}

VECTOR Player::CalculateCenterOfMass() const
{
	VECTOR sum = VGet(0.0f, 0.0f, 0.0f);
	int activeCount = 0;

	for (int r = 0; r < GameConfig::PLAYER_PIECE_SIZE; ++r)
	{
		for (int c = 0; c < GameConfig::PLAYER_PIECE_SIZE; ++c)
		{
			auto piece = mPieces[c][r];
			if (!piece || !piece->IsActive())
				continue;

			if (auto pieceTransform = piece->GetModule<Transform>())
			{
				sum = VAdd(sum, pieceTransform->GetPosition());
				++activeCount;
			}
		}
	}

	if (activeCount > 0)
		return VScale(sum, 1.0f / static_cast<float>(activeCount));

	// 有効Pieceが無いときは Player の座標
	if (auto transform = GetTransform())
		return transform->GetPosition();

	return sum;
}

Transform* Player::GetTransform() const
{
	// GetModule が非constのため、const関数からも呼べるようにここだけでキャストする
	return const_cast<Player*>(this)->GetModule<Transform>();
}

// ============================================================ //
//  リスポーン
// ============================================================ //
void Player::CheckRespawn()
{
	auto transform = GetTransform();
	if (!transform)
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
	//// -- 回転値を取得 -- //
	//float rotationAmount = _rotateDirection * mfRotationPower;	// 回転の値(度数)
	//float rotationRadian = rotationAmount * DX_PI_F / 180.0f;	// 回転の値(ラジアン)

	//// -- 回転値を適用 -- //
	//if (auto transform = GetModule<Transform>())
	//	transform->SetRotation(
	//		VAdd(
	//			transform->GetRotation(),
	//			VGet(0.0f, 0.0f, rotationRadian)
	//		)
	//	);

	//UpdatePieceRotation();

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

	// SE_MOVE を再生
	SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_MOVE);
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
	auto transform = GetTransform();
	if (!transform)
		return;

	transform->SetPosition(mvRespawnPos);
	transform->SetRotation(VGet(0.0f, 0.0f, 0.0f));
	mbIsToppling = false;
	mfInputCooldown = 0.0f;
	SetGroundedState(false);

	UpdatePieceTransforms();
}
