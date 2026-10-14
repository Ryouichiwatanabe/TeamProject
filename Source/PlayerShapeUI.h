#pragma once
#include "UI.h"

#include "GameConfig.h"

class PlayerPieceUI;
class Player;	// PlayerPieceの有効状態を見る用

class PlayerShapeUI : public UI
{
public:
	PlayerShapeUI() = default;
	~PlayerShapeUI() = default;

	void Init() override;
	void Update() override;
	void Draw() override;

	bool IsPieceSelected(int _x, int _y);
	void SetPieceSelected(int _x, int _y, bool _enabled);
private:
	void CreatePieceUI(int _column, int _row);

	void ApplyShapeFromUI();

private:
	Player* mpPlayer;

	PlayerPieceUI* mPieceUI
		[GameConfig::PLAYER_PIECE_SIZE]
		[GameConfig::PLAYER_PIECE_SIZE];

	UI* mpConfirmUI;
	UI* mpBackUI;

};