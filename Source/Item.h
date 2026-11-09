// Item.h
#pragma once
#include "GameObject.h"

class Item : public GameObject {
public:
	Item(int _stage, int _slot) : mStage(_stage), mSlot(_slot) {}
	~Item() = default;

	void Init() override;
	void InitComponent() override;
	void Update(float _deltaTime) override;

	bool IsPicked() const { return mState != State::Idle; }
	int GetSlot() const { return mSlot; }

private:
	enum class State { Idle, Picking };	// 待機中 / 取得アニメ中

	void UpdateIdle();
	void UpdatePicking(float _deltaTime);

	static constexpr float PICK_DURATION = 0.25f;

	int mStage;
	int mSlot;
	State mState = State::Idle;
	float mTimer = 0.0f;
};