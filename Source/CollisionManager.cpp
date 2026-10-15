#include "CollisionManager.h"

#include "Collider.h"
#include "Transform.h"

#include <DxLib.h>	// デバッグ表示のカラー指定のため使用
#include <cmath>
#include <algorithm>

#include "Debug.h"

void CollisionManager::Finalize() 
{
	// -- 当たり判定データのコンテナを解放 -- //
	for (auto* collider : mColliders)
	{
		collider->ClearCollisions();
	}

	mColliders.clear();
}

void CollisionManager::Update() 
{
	// 前フレームの衝突結果をリセット
	for (auto* collider : mColliders) 
	{
		collider->ClearCollisions();
	}

	// 判定を行うColliderを選び、衝突結果を各自処理するコンテナに登録
	for (size_t i = 0;
		i < mColliders.size();
		++i)
	{
		Collider* a = mColliders[i];
		
		if (!a->IsEnabled()) continue;

		for (size_t j = i + 1;
			j < mColliders.size();
			++j) 
		{
			Collider* b = mColliders[j];

			if (!b->IsEnabled()) continue;

			if (CheckBoxBox(a, b)) 
			{
				a->AddCollision(b);
				b->AddCollision(a);
			}
		}
	}
}

void CollisionManager::Register(Collider* _collider) 
{
	if (_collider == nullptr) return;

	for (auto* collider : mColliders)
	{
		if (collider == _collider) return;
	}

	mColliders.push_back(_collider);
}

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
	Collider* _boxA,
	Collider* _boxB
) const
{
	if (_boxA == nullptr || _boxB == nullptr)
		return false;

	// -- Transform取得 -- //
	auto transformA =
		_boxA->GetGameObject()->GetModule<Transform>();

	auto transformB =
		_boxB->GetGameObject()->GetModule<Transform>();

	if (transformA == nullptr || transformB == nullptr)
		return false;

	// -- 各衝突範囲を取得 -- //
	const VECTOR posA =
		_boxA->GetWorldPosition();

	const VECTOR posB =
		_boxB->GetWorldPosition();

	const VECTOR halfA =
		_boxA->GetHalfSize();

	const VECTOR halfB =
		_boxB->GetHalfSize();

	// -- 回転値取得 -- //
	const float rotA =
		transformA->GetRotation().z;

	const float rotB =
		transformB->GetRotation().z;

	const VECTOR axisAX = VGet(
		cosf(rotA),
		sinf(rotA),
		0.0f
	);

	const VECTOR axisAY = VGet(
		-sinf(rotA),
		cosf(rotA),
		0.0f
	);

	const VECTOR axisBX = VGet(
		cosf(rotB),
		sinf(rotB),
		0.0f
	);

	const VECTOR axisBY = VGet(
		-sinf(rotB),
		cosf(rotB),
		0.0f
	);

	const VECTOR centerDiff =
		VSub(posB, posA);

	const VECTOR axes[] =
	{
		axisAX,
		axisAY,
		axisBX,
		axisBY
	};

	for (const VECTOR& axis : axes)
	{
		// 中心間距離を軸へ投影
		const float distance =
			fabsf(VDot(centerDiff, axis));

		// Aの投影半径
		const float radiusA =
			halfA.x * fabsf(VDot(axisAX, axis)) +
			halfA.y * fabsf(VDot(axisAY, axis));

		// Bの投影半径
		const float radiusB =
			halfB.x * fabsf(VDot(axisBX, axis)) +
			halfB.y * fabsf(VDot(axisBY, axis));

		// 分離している
		if (radiusA + radiusB <= distance)
		{
			return false;
		}
	}

	const float minAZ =
		posA.z - halfA.z;

	const float maxAZ =
		posA.z + halfA.z;

	const float minBZ =
		posB.z - halfB.z;

	const float maxBZ =
		posB.z + halfB.z;

	if (maxAZ < minBZ ||
		minAZ > maxBZ)
	{
		return false;
	}

	return true;
}

bool CollisionManager::GetBoxBoxCollision(
	Collider* _a,
	Collider* _b,
	CollisionInfo& _info
) const
{
	if (_a == nullptr || _b == nullptr) 
		return false;

	// -- 角度取得のためのnullCheck -- //
	auto transformA = _a->GetGameObject()->GetModule<Transform>();
	auto transformB = _b->GetGameObject()->GetModule<Transform>();
	if (transformA == nullptr || transformB == nullptr)
		return false;

	// -- Box対Boxの各衝突受付範囲を取得 -- //
	const VECTOR posA = _a->GetWorldPosition();
	const VECTOR posB = _b->GetWorldPosition();

	const VECTOR halfA = _a->GetHalfSize();
	const VECTOR halfB = _b->GetHalfSize();

	const float rotA = transformA->GetRotation().z;
	const float rotB = transformB->GetRotation().z;

	// Aのローカル軸
	const VECTOR axisAX = VGet(
		cosf(rotA),
		sinf(rotA),
		0.0f
	);

	const VECTOR axisAY = VGet(
		-sinf(rotA),
		cosf(rotA),
		0.0f
	);

	// Bのローカル軸
	const VECTOR axisBX = VGet(
		cosf(rotB),
		sinf(rotB),
		0.0f
	);

	const VECTOR axisBY = VGet(
		-sinf(rotB),
		cosf(rotB),
		0.0f
	);

	// Z方向の判定
	/*if (fabsf(posA.z - posB.z) > halfA.z + halfB.z)
	{
		return false;
	}*/

	// 中心間ベクトル
	const VECTOR centerDiff = VSub(posB, posA);

	// 判定する軸
	const VECTOR axes[] =
	{
		axisAX,
		axisAY,
		axisBX,
		axisBY
	};

	float minPenetration = FLT_MAX;
	VECTOR collisionNormal = VGet(0.0f, 0.0f, 0.0f);

	for (const VECTOR& axis : axes)
	{
		// 中心間距離をこの軸へ投影
		const float distance =
			fabsf(
				VDot(centerDiff, axis)
			);

		// Aをこの軸へ投影した半径
		const float radiusA =
			halfA.x * fabsf(VDot(axisAX, axis)) +
			halfA.y * fabsf(VDot(axisAY, axis));

		// Bをこの軸へ投影した半径
		const float radiusB =
			halfB.x * fabsf(VDot(axisBX, axis)) +
			halfB.y * fabsf(VDot(axisBY, axis));

		// 分離している
		const float overlap =
			radiusA + radiusB - distance;

		if (overlap <= 0.0f)
		{
			return false;
		}

		// 最も浅いめり込みを記録
		if (overlap < minPenetration)
		{
			minPenetration = overlap;

			collisionNormal = axis;

			// B -> A の方向に法線を向ける
			if (VDot(centerDiff, collisionNormal) > 0.0f)
			{
				collisionNormal =
					VScale(collisionNormal, -1.0f);
			}
		}
	}

	Debug::Print(
		"A:(", posA.x, ",", posA.y, ")", "B:(", posB.x, ",", posB.y, ")", "Pen :", minPenetration);

	Debug::Print(
		"HalfA:(", halfA.x, ",", halfA.y, ")", "HalfB:(", halfB.x, ",", halfB.y, ")");

	_info.penetration = minPenetration;
	_info.normal = collisionNormal;

	return true;
}