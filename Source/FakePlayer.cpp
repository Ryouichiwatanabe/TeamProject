#include "FakePlayer.h"

// -- Module -- //
#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"
#include "Gravity.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "CollisionManager.h"

#include "InputManager.h"

#include "GameConfig.h"

void FakePlayer::Init()
{
	mController.Initialize(this);
}

void FakePlayer::InitComponent()
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

void FakePlayer::Finalize()
{

}

void FakePlayer::Update(float _deltaTime)
{
	GameObject::Update(_deltaTime);
	mController.Update();

}

void FakePlayer::Draw()
{
	GameObject::Draw();
}

void FakePlayer::Move(VECTOR _direction)
{
	
	// -- 移動量を取得 & 座標反映 -- //
	VECTOR moveAmount = VScale(_direction, mfSpeed);

	auto transform = GetModule<Transform>();
	VECTOR nextPos = VAdd(
		transform->GetPosition(),
		moveAmount
	);

	transform->SetPosition(nextPos);
}

void FakePlayer::Rotation(float _rotateDirection)
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
}

void FakePlayer::ResolveCollision()
{
	GameObject::ResolveCollision();

	auto transform = GetModule<Transform>();
	if (!transform)
		return;
	VECTOR position = transform->GetPosition();

	position = VAdd(
		position,
		this->GetCollisionCorrection()
	);

	transform->SetPosition(position);
}