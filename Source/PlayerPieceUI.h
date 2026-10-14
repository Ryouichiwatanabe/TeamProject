#pragma once
#include "UI.h"

class PlayerPiece;	// PlayerPieceの有効状態を操作するために宣言

class PlayerPieceUI : public UI
{
public:
	PlayerPieceUI() = default;
	~PlayerPieceUI() = default;

	void Init() override;
	void Update() override;
	void Draw() override;

	void SetPlayerPiece(PlayerPiece* _piece) { mpPlayerPiece = _piece; }

	// -- 選択状態のアクセサ -- //
	bool IsSelected();
	void SetSelected(bool _selected);
	void ToggleSelected();

private:
	PlayerPiece* mpPlayerPiece = nullptr;

	bool mbSelected = false;
};