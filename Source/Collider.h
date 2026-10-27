#pragma once
#include "Module.h"

#include "GameObject.h"	// Tag参照用。循環参照に注意

#include <DxLib.h>
#include <vector>
#include "Box2D.h"

class CollisionManager;

class Collider : public Module
{
public:
	 ~Collider() = default;

	 void Initialize() override;	// 初期処理
	 void Finalize() override;		// 終了処理

	// -- Objの位置(Transform)からの相対オフセット位置アクセサ -- //
	void SetOffset(const VECTOR& _offset) { mvOffSetPos = _offset; }
	const VECTOR& GetOffset() const { return mvOffSetPos; }

	const VECTOR& GetHalfSize() const { return mvHalfSize; }
	void SetHalfSize(const VECTOR& _halfSize) { mvHalfSize = _halfSize; }

	VECTOR GetWorldPosition() const;	// Collider中心座標(Transform + Offset)を返す
	Box2D GetBox2D() const;				// 回転を考慮したBox2Dデータを返す

	bool IsColliding() const;				// 何かしらに当たったら反応
	bool IsColliding(Tag _tag) const;		// 指定タグのColliderに衝突時、反応
	std::vector<GameObject*> GetCollisions(Tag _tag);	// 指定タグの衝突したGameObjectを返す(複数対応)

	// -- 衝突Objの指定関数 - //
	void AddCollisionTag(Tag _tag) { mCollisionTags.push_back(_tag); }
	std::vector<Tag> GetCollisionTag() { return mCollisionTags; }

private:
	friend class CollisionManager;

	void AddCollision(Collider* _collider) { mCollisions.push_back(_collider); }
	void ClearCollisions() { mCollisions.clear(); }

private:
	VECTOR mvOffSetPos = VGet(0.0f, 0.0f, 0.0f);	// Objの位置(Transform)からの相対オフセット位置
	VECTOR mvHalfSize = VGet(0.5f, 0.5f, 0.0f);

	std::vector<Collider*> mCollisions;	// 指定フレームで衝突している他Colliderのコンテナ
	std::vector<Tag> mCollisionTags;	// 衝突判定を行うObjのタグコンテナ

	bool mbEnabled = true;	// 有効を示すフラグ
};