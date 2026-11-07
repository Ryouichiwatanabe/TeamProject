#include "Item.h"

// -- Itemデータ -- //
#include "ItemTable.h"

// -- Module -- //
#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

// -- Config -- //
#include "GameConfig.h"

void Item::InitComponent() {
	AddModule<Transform>();
	AddModule<Graphic>(ItemTable::GetFilePath(mStage, mSlot));
	auto collider = AddModule<Collider>();
	collider->SetHalfSize(VGet(GameConfig::CELL_SIZE / 4.0f, 25.0f, 0.0f));
}

void Item::Init() {
	SetTag(Tag::ITEM);

	if (ItemTable::Find(mStage, mSlot) == nullptr) {
		Destroy();
		return;
	}
	GetModule<Collider>()->AddCollisionTag(Tag::PLAYER_CELL);
}

void Item::Update(float _deltaTime) {
	if (_deltaTime <= 0.0f)		// Pause中は止まる
		return;

	switch (mState) {
	case State::Idle:    UpdateIdle();              break;
	case State::Picking: UpdatePicking(_deltaTime); break;
	}
	GameObject::Update(_deltaTime);
}

void Item::UpdateIdle() {
	auto collider = GetModule<Collider>();
	if (!collider || !collider->IsColliding(Tag::PLAYER_CELL))
		return;

	mState = State::Picking;	// IsPicked() が true になる
	mTimer = 0.0f;
	collider->SetEnabled(false);
}

void Item::UpdatePicking(float _deltaTime) {
	mTimer += _deltaTime;
	const float rate = (std::min)(mTimer / PICK_DURATION, 1.0f);

	// 前半：1.0→1.3倍 / 後半：1.3→0倍
	const float scale = (rate < 0.5f)
		? 1.0f + (rate / 0.5f) * 0.3f
		: 1.3f * (1.0f - (rate - 0.5f) / 0.5f);

	if (auto transform = GetModule<Transform>())
		transform->SetScale(VGet(scale, scale, 1.0f));

	if (rate >= 1.0f)
		Destroy();
}