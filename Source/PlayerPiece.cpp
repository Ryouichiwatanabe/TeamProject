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

void PlayerPiece::SetActive(bool _active)
{
	mbIsActive = _active;

	if (auto transform = GetModule<Transform>())
		transform->SetEnabled(_active);
	
	if (auto graphic = GetModule<Graphic>())
		graphic->SetEnabled(_active);

	if (auto collider = GetModule<Collider>())
		collider->SetEnabled(_active);
}

void PlayerPiece::ResolveCollision()
{
	GameObject::ResolveCollision();

	if (mpPlayer == nullptr)
		return;

	// 押し戻し補正値を親Playerへ通知
	mpPlayer->AddCollisionCorrection(GetCollisionCorrection());

	// 詳細な接触情報（法線、侵入深度、接触ブロック）を親Playerへ通知
	auto myCollider = GetModule<Collider>();
	auto myTransform = GetModule<Transform>();
	if (!myCollider || !myTransform || !myCollider->IsEnabled())
		return;

	auto scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
	auto collisionManager = scene ? scene->GetCollisionManager() : nullptr;
	if (!collisionManager)
		return;

	auto blocks = myCollider->GetCollisions(Tag::BLOCK);
	for (auto block : blocks)
	{
		if (block == nullptr)
			continue;

		auto blockCollider = block->GetModule<Collider>();
		if (blockCollider == nullptr)
			continue;

		CollisionInfo info;
		if (collisionManager->GetBoxBoxCollision(myCollider, blockCollider, info))
		{
			PieceContact contact;
			contact.mpPiece = this;
			contact.mpOtherBlock = block;
			contact.mCollisionInfo = info;
			contact.mvPieceWorldPos = myTransform->GetPosition();
			mpPlayer->AddPieceContact(contact);
		}
	}
}