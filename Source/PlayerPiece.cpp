#include "PlayerPiece.h"

// Module関係
#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"
#include "Gravity.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "CollisionManager.h"

// ハブPlayer参照用
#include "Player.h"

#include "GameConfig.h"

PlayerPiece::PlayerPiece(Player* _player)
	: GameObject()
{
	// -- ハブのPlayerを設定 -- //
	mpPlayer = _player;
}

void PlayerPiece::Init()
{
	// -- タグ設定 -- //
	SetTag(Tag::PLAYER_CELL);
}

void PlayerPiece::InitComponent()
{
	// -- Module追加 -- //
	AddModule<Transform>();
	AddModule<Graphic>(GetModelFilename());
	AddModule<Collider>();
	AddModule<Gravity>();

	// -- 衝突判定を取得するObjを指定 -- //
	auto collider = GetModule<Collider>();
	collider->AddCollisionTag(Tag::BLOCK);
	collider->SetHalfSize(VGet(GameConfig::CELL_SIZE / 2.0f, GameConfig::CELL_SIZE / 2.0f, 0.0f));
}

void PlayerPiece::Update(float _deltaTime)
{
	GameObject::Update(_deltaTime);
}

void PlayerPiece::Draw()
{
	GameObject::Draw();
}

void PlayerPiece::SetEnabled(bool _enabled)
{
	mbIsEnabled = _enabled;

	if (auto transform = GetModule<Transform>())
		transform->SetEnabled(_enabled);
	
	if (auto graphic = GetModule<Graphic>())
		graphic->SetEnabled(_enabled);

	if (auto collider = GetModule<Collider>())
		collider->SetEnabled(_enabled);

	if (auto gravity = GetModule<Gravity>())
		gravity->SetEnabled(_enabled);
}

void PlayerPiece::ResolveCollision()
{
	GameObject::ResolveCollision();

	mpPlayer->AddCollisionCorrection(GetCollisionCorrection());
}

void PlayerPiece::UpdateWorldPosition()
{
	if (mpPlayer == nullptr)
		return;

	// -- 更新先の座標を取得 -- //
	auto player_transform = mpPlayer->GetModule<Transform>();
	VECTOR nextPos = VAdd(
		player_transform->GetPosition(),
		mvLocalPosition
	);

	// -- 座標を更新 -- //
	auto piece_transform = this->GetModule<Transform>();
	piece_transform->SetPosition(nextPos);
}

void PlayerPiece::UpdateRotation()
{
	auto playerTransform = mpPlayer->GetModule<Transform>();
	auto pieceTransform = GetModule<Transform>();

	if (playerTransform == nullptr || pieceTransform == nullptr)
		return;

	// Playerの回転角度（Z軸）
	const float angle = playerTransform->GetRotation().z;

	const float cosAngle = cosf(angle);
	const float sinAngle = sinf(angle);

	// Playerの中心を基準に、Pieceのローカル座標を回転
	VECTOR rotatedLocalPosition = VGet(
		mvLocalPosition.x * cosAngle - mvLocalPosition.y * sinAngle,
		mvLocalPosition.x * sinAngle + mvLocalPosition.y * cosAngle,
		mvLocalPosition.z
	);

	// 回転後のローカル座標をワールド座標へ変換
	VECTOR worldPosition = VAdd(
		playerTransform->GetPosition(),
		rotatedLocalPosition
	);

	pieceTransform->SetPosition(worldPosition);

	// Piece自体の見た目もPlayerと同じ角度にする
	pieceTransform->SetRotation(playerTransform->GetRotation());
}