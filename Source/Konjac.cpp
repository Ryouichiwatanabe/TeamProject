#include "Konjac.h"
#include "Player.h"
#include "PlayerPiece.h"

// -- Module -- //
#include "Transform.h"
#include "Collider.h"

// -- Manager -- //
#include "Master.h"
#include "ResourceManager.h"
#include "ObjectManager.h"
#include "GameConfig.h"

#include <cmath>
#include <vector>

void Konjac::Init()
{
	StageBlock::Init();

	// -- 揺れ描画用に画像を取得(ResourceManagerがキャッシュしているのでGraphicと同じハンドル) -- //
	mnGraphHandle = Master::mpResource->LoadGraphics(GetModelFilename());

	int sizeX = 0;
	int sizeY = 0;
	GetGraphSize(mnGraphHandle, &sizeX, &sizeY);
	mvGraphSize = VGet(static_cast<float>(sizeX), static_cast<float>(sizeY), 0.0f);
}

void Konjac::Update(float _deltaTime)
{
	if (_deltaTime <= 0.0f)
		return;

	if (mfBounceTimer > 0.0f)
		mfBounceTimer -= _deltaTime;

	if (mfCooldown > 0.0f)
		mfCooldown -= _deltaTime;

	TryStartJump();
	UpdateJumpCarry(_deltaTime);

	StageBlock::Update(_deltaTime);
}

void Konjac::Draw()
{
	// -- 揺れていないときは通常どおりGraphicモジュールで描画 -- //
	if (mfBounceTimer <= 0.0f)
	{
		StageBlock::Draw();
		return;
	}

	DrawBounce();
}

// =============================== 跳ね上げ =============================== //

void Konjac::TryStartJump()
{
	if (mfCooldown > 0.0f)
		return;

	Player* player = FindPlayerOnTop();
	if (player == nullptr)
		return;

	mpJumpingPlayer = player;
	mfJumpSpeedY = INITIAL_JUMP_SPEED;
	mfBounceTimer = BOUNCE_ANIM_TIME;
	mfCooldown = COOLDOWN_TIME;
}

Player* Konjac::FindPlayerOnTop()
{
	auto* myTransform = GetModule<Transform>();
	auto* objectManager = GetObjectManager();
	if (myTransform == nullptr || objectManager == nullptr)
		return nullptr;

	const VECTOR myPos = myTransform->GetPosition();
	const float halfCell = GameConfig::CELL_SIZE * 0.5f;
	const float myTop = myPos.y - halfCell;

	for (auto* piece : objectManager->FindObjects<PlayerPiece>())
	{
		if (!piece->IsActive())
			continue;

		auto* pieceTransform = piece->GetModule<Transform>();
		if (pieceTransform == nullptr)
			continue;

		const VECTOR piecePos = pieceTransform->GetPosition();
		const float pieceBottom = piecePos.y + halfCell;

		const bool isWithinX =
			std::fabsf(piecePos.x - myPos.x) < GameConfig::CELL_SIZE * ON_TOP_RANGE_X_RATE;
		const bool isTouchingTop =
			pieceBottom >= myTop - ON_TOP_ABOVE &&
			pieceBottom <= myTop + ON_TOP_BELOW;

		if (isWithinX && isTouchingTop)
			return piece->GetPlayer();
	}

	return nullptr;
}

void Konjac::UpdateJumpCarry(float _deltaTime)
{
	if (mpJumpingPlayer == nullptr)
		return;

	auto* playerTransform = mpJumpingPlayer->GetModule<Transform>();
	const float stepY = -mfJumpSpeedY * _deltaTime;

	// -- Transformが無い / 天井にぶつかる場合は跳ね上げ終了 -- //
	if (playerTransform == nullptr || HasCeilingAbovePlayer(mpJumpingPlayer, stepY))
	{
		mfJumpSpeedY = 0.0f;
		mpJumpingPlayer = nullptr;
		return;
	}

	VECTOR pos = playerTransform->GetPosition();
	pos.y += stepY;
	playerTransform->SetPosition(pos);
	mpJumpingPlayer->UpdatePieceTransforms();

	// -- 減速し、速度が0になったら終了 -- //
	mfJumpSpeedY -= JUMP_DECEL * _deltaTime;
	if (mfJumpSpeedY <= 0.0f)
	{
		mfJumpSpeedY = 0.0f;
		mpJumpingPlayer = nullptr;
	}
}

bool Konjac::HasCeilingAbovePlayer(Player* _player, float _stepY)
{
	auto* objectManager = GetObjectManager();
	if (objectManager == nullptr)
		return false;

	const float halfCell = GameConfig::CELL_SIZE * 0.5f;
	const float rangeX = GameConfig::CELL_SIZE * ON_TOP_RANGE_X_RATE;

	// -- 対象プレイヤーの有効なPiece座標を先に集める -- //
	std::vector<VECTOR> piecePositions;
	for (auto* piece : objectManager->FindObjects<PlayerPiece>())
	{
		if (!piece->IsActive() || piece->GetPlayer() != _player)
			continue;

		if (auto* transform = piece->GetModule<Transform>())
			piecePositions.push_back(transform->GetPosition());
	}

	// -- 各ブロックについて、Pieceの移動先が下面に食い込むか調べる -- //
	for (auto* obj : objectManager->GetObjects())
	{
		if (obj == nullptr || obj == this || obj->GetTag() != Tag::BLOCK)
			continue;

		auto* blockCollider = obj->GetModule<Collider>();
		auto* blockTransform = obj->GetModule<Transform>();
		if (blockCollider == nullptr || blockTransform == nullptr || !blockCollider->IsEnabled())
			continue;

		const VECTOR blockPos = blockTransform->GetPosition();
		const float blockBottom = blockPos.y + halfCell;

		for (const auto& piecePos : piecePositions)
		{
			const float nextTop = piecePos.y + _stepY - halfCell;

			const bool isWithinX = std::fabsf(piecePos.x - blockPos.x) < rangeX;
			const bool isBelowBlock = piecePos.y >= blockPos.y;

			if (isWithinX && isBelowBlock && nextTop <= blockBottom)
				return true;
		}
	}

	return false;
}

// =============================== 描画 =============================== //

void Konjac::DrawBounce()
{
	auto* transform = GetModule<Transform>();
	if (transform == nullptr || mnGraphHandle == -1)
		return;

	// -- 減衰する正弦波で縦横を逆向きに伸縮させる(プルンプルン) -- //
	const float t = BOUNCE_ANIM_TIME - mfBounceTimer;
	const float wave = std::sinf(t * BOUNCE_FREQUENCY) * std::expf(-t * BOUNCE_DAMPING);
	const float stretchX = 1.0f - wave * BOUNCE_STRETCH_X;
	const float stretchY = 1.0f + wave * BOUNCE_STRETCH_Y;

	const VECTOR pos = transform->GetPosition();
	const VECTOR baseScale = transform->GetScale();	// Graphic::Initialize で CELL_SIZE に合わせた拡縮値
	const float halfCell = GameConfig::CELL_SIZE * 0.5f;

	// -- 画像の「下辺中央」を基準点にして、ブロックの底面に合わせて描画 -- //
	DrawRotaGraph3(
		static_cast<int>(pos.x),
		static_cast<int>(pos.y + halfCell),
		static_cast<int>(mvGraphSize.x / 2.0f),
		static_cast<int>(mvGraphSize.y),
		baseScale.x * stretchX,
		baseScale.y * stretchY,
		transform->GetRotation().z,
		mnGraphHandle,
		TRUE,
		FALSE
	);
}
