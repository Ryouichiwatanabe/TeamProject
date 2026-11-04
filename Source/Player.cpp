#include "Player.h"

/* memo :
* Player の「基本」部分。
*   生成(Init) → 毎フレームの更新(Update) → Pieceの座標同期 → リスポーン
* 回転・接地・形状変更は別ファイル(Player.h の memo 参照)。
*/

// -- Module -- //
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

#include "SoundManager.h"

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

	if (transform->GetPosition().y > ScreenConfig::SCREEN_HEIGHT)
		Respawn();
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
