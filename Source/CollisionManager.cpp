#include "CollisionManager.h"

#include "Collider.h"
#include "Transform.h"

#include <DxLib.h>	// デバッグ表示のカラー指定のため使用
#include <cmath>
#include <algorithm>
#include <cfloat>	// FLT_MAX

#include "Debug.h"

void CollisionManager::Finalize() 
{
	// -- 当たり判定データのコンテナを解放 -- //
	for (auto* collider : mColliders){
		collider->ClearCollisions();
	}

	mColliders.clear();
}

namespace
{
	// _collider の判定対象タグに _other のタグが含まれているか
	bool IsCollisionTarget(Collider* _collider, GameObject* _other)
	{
		for (auto tag : _collider->GetCollisionTag()) {
			if (_other->GetTag() == tag)
				return true;
		}
		return false;
	}
}

void CollisionManager::Update() {
	// 前フレームの衝突結果をリセット
	for (auto* collider : mColliders) {
		collider->ClearCollisions();
	}

	// 判定を行うColliderを選び、衝突結果を各自処理するコンテナに登録
	for (size_t i = 0; i < mColliders.size(); ++i) {
		Collider* a = mColliders[i];
		if (!a || !a->IsEnabled())
			continue;

		for (size_t j = i + 1; j < mColliders.size(); ++j) {
			Collider* b = mColliders[j];
			if (!b || !b->IsEnabled())
				continue;

			auto aObj = a->GetGameObject();
			auto bObj = b->GetGameObject();
			if (!aObj || !bObj)
				continue;

			// どちらか一方でも相手を判定対象にしていれば判定する
			const bool checkNeeded = IsCollisionTarget(a, bObj) || IsCollisionTarget(b, aObj);

			if (checkNeeded && CheckBoxBox(a, b)) {
				a->AddCollision(b);
				b->AddCollision(a);
			}
		}
	}
}

// -- 呼び出し先のColliderをManagerに登録 -- //
void CollisionManager::Register(Collider* _collider) 
{
	if (_collider == nullptr) return;

	for (auto* collider : mColliders)
	{
		if (collider == _collider) return;
	}

	mColliders.push_back(_collider);
}

// -- 呼び出し先のColliderをManagerから除外 -- //
void CollisionManager::Unregister(Collider* _collider)
{
	for (auto collider = mColliders.begin();
		collider != mColliders.end();
		++collider)
	{
		if (*collider == _collider)
		{
			mColliders.erase(collider);
			return;
		}
	}
}

bool CollisionManager::CheckBoxBox(
	Collider* _colliderA, 
	Collider* _colliderB
) {
	CollisionInfo info;
	return GetBoxBoxCollision(_colliderA, _colliderB, info);
}

bool CollisionManager::GetBoxBoxCollision(
	Collider* _colliderA,
	Collider* _colliderB,
	CollisionInfo& _info
) {
	if (!_colliderA || !_colliderB || !_colliderA->IsEnabled() || !_colliderB->IsEnabled())
		return false;

	return TestBoxBox(_colliderA->GetBox2D(), _colliderB->GetBox2D(), _info);
}

bool CollisionManager::TestBoxBox(
	const Box2D& _boxA,
	const Box2D& _boxB,
	CollisionInfo& _info
) {
	const VECTOR axes[4] = {
		_boxA.axisX,
		_boxA.axisY,
		_boxB.axisX,
		_boxB.axisY
	};

	const VECTOR delta = VSub(_boxB.position, _boxA.position);

	float minOverlap = FLT_MAX;
	VECTOR bestAxis = VGet(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		VECTOR axis = axes[i];
		const float lenSq = axis.x * axis.x + axis.y * axis.y;
		if (lenSq < 0.00001f) continue;
		const float invLen = 1.0f / std::sqrtf(lenSq);
		axis.x *= invLen;
		axis.y *= invLen;

		const float rA = _boxA.ProjectToAxis(axis);
		const float rB = _boxB.ProjectToAxis(axis);

		const float dist = std::fabsf(delta.x * axis.x + delta.y * axis.y);
		const float overlap = (rA + rB) - dist;

		if (overlap <= 0.0f) {
			// 分離軸が存在するため重なっていない
			return false;
		}

		if (overlap < minOverlap) {
			minOverlap = overlap;
			bestAxis = axis;
		}
	}

	// 法線の向きを調整: Aから見てBから押し離す方向（delta = B - A なので、deltaと同方向なら反転）
	const float dot = delta.x * bestAxis.x + delta.y * bestAxis.y;
	if (dot > 0.0f) {
		bestAxis.x = -bestAxis.x;
		bestAxis.y = -bestAxis.y;
	}

	_info.penetration = minOverlap;
	_info.normal = bestAxis;
	return true;
}