#pragma once
#include "GameObject.h"

#include <DxLib.h>	// VECTOR用

class Player;
class CollisionManager;

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
	bool IsActive() const { return mbIsActive; }

	// -- Playerからの相対座標アクセサ -- //
	void SetLocalPosition(const VECTOR& _pos) { mvLocalPosition = _pos; }
	const VECTOR& GetLocalPosition() const { return mvLocalPosition; }

	Player* GetPlayer() const { return mpPlayer; }

protected:
	const char* GetModelFilename() const override
	{
		return "Resource/GameObject/Player.png";
	}
private:
	Player* mpPlayer = nullptr;	// ハブとなるPlayerのポインタ

	VECTOR mvLocalPosition = VGet(0.0f, 0.0f, 0.0f);		// Playerからの相対座標
	bool mbIsActive = false;	// Pieceの有効状態を示す
};