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

	void ApplyPieceActivityToUI();	// UI展開時にPlayerPieceの有効状態を取得し、UIPlayerPieceに適用させる処理

	// -- PieceUIの取得する -- //
	PlayerPieceUI* GetPlayerPieceUI(int _column, int _row) { return mPieceUI[_column][_row]; }
private:
	void CreatePieceUI(int _column, int _row);

private:
	Player* mpPlayer;

	PlayerPieceUI* mPieceUI
		[GameConfig::PLAYER_PIECE_SIZE]
		[GameConfig::PLAYER_PIECE_SIZE];

	UI* mpConfirmUI;
	UI* mpBackUI;

};