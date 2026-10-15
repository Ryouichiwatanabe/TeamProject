#pragma once
#include "GameObject.h"

#include <DxLib.h>	// VECTOR用

class Player;

class PlayerPiece : public GameObject
{
public:
	PlayerPiece(Player* _player);
	~PlayerPiece() = default;

	void Init() override;
	void InitComponent() override;
	void Update(float _deltaTime) override;
	void Draw() override;
	void ResolveCollision() override;

	// -- 有効状態のアクセサ -- //
	void SetActive(bool _active);
	bool IsActive() { return mbIsActive; }

	// -- Playerからの相対座標アクセサ -- //
	void SetLocalPosition(VECTOR _pos) { mvLocalPosition = _pos; }
	VECTOR GetLocalPosition() { return mvLocalPosition; }

	void UpdateWorldPosition();		// Playerに基づいた相対座標へ座標更新
	void UpdateRotation();
protected:
	const char* GetModelFilename() const override
	{
		return "Resource/Player_test.png";
	}
private:
	Player* mpPlayer = nullptr;	// ハブとなるPlayerのポインタ

	VECTOR mvLocalPosition = VGet(0.0f, 0.0f, 0.0f);		// Playerからの相対座標

	// Todo : Playerの初期配列には関与しておらず、
		// これを基にPieceの有効状態を変更しているわけでもない(主にSetEnabled基準)ので完全にGoal用。
		// いつか直す
	bool mbIsActive = false;	// Pieceの有効状態を示す
};