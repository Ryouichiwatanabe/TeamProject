#pragma once
#include <DxLib.h>	// VECTOR
#include <vector>	// std::vector
#include "Box2D.h"

class Collider;
class GameObject;	// Tag用

struct CollisionInfo
{
	VECTOR normal;				// 押し出す方向
	float penetration = 0.0f;	// どれだけめり込んでいるか
};

/* memo : 
* 各Objが持つ、「Colliderクラス」を総括して管理するクラス。
* 主に毎フレーム全Collider同士の衝突判定を行い、
*	各Colliderへ「衝突しているCollider」のデータを渡す。
* 
* 所有権はScene。
* 
* 9/8時点では当たり判定は「Box型」のみ
*/

class CollisionManager
{
public:
	
public:
	CollisionManager() = default;
	~CollisionManager() = default;
	
	void Finalize();
	void Update();

	void Register(Collider* _collider);			// 呼び出し先のColliderをManagerに登録
	void Unregister(Collider* _collider);		// 呼び出し先のColliderをManagerから除外

	bool CheckBoxBox(Collider* _colliderA, Collider* _colliderB);	// 同フレームで衝突したColliderを登録する
	bool GetBoxBoxCollision(Collider* _colliderA, Collider* _colliderB, CollisionInfo& _info);

	// Box2D同士のSAT衝突判定（めり込み量・押し戻し法線を算出）
	static bool TestBoxBox(const Box2D& _boxA, const Box2D& _boxB, CollisionInfo& _info);

private:
	std::vector<Collider*> mColliders;	// 使用されている当たり判定データのコンテナ
};