#pragma once
#include "UI.h"

#include "GameConfig.h"

#include <string>

class PlayerPieceUI;
class Player;

/* memo :
* プレイヤーの形状変更UI(外枠 + 3×3の内側マス + OK/Backボタン)。
* 役割は「表示と入力」だけ。形状が正しいかの判定・適用は Player 側が行う。
*
* 使い方:
*   Open()  … 現在のPlayerの形をUIに反映して表示(Playerは編集中で停止)
*   Close() … 非表示にしてPlayerの停止を解除
*/
class PlayerShapeUI : public UI
{
public:
	PlayerShapeUI() = default;
	~PlayerShapeUI() = default;

	void Init() override;
	void Update() override;
	void Draw() override;

	void Open();
	void Close();

	// _column : 横(x) / _row : 縦(y)。どちらも画面の見た目どおりの向き
	bool IsPieceSelected(int _column, int _row) const;

private:
	void CreatePieceUI(int _column, int _row);
	UI* CreateButtonUI(const std::string& _text, float _x, float _y);
	UI* CreateMessageUI(float _x, float _y);

	void SyncFromPlayer();		// Playerの現在形状 → UI
	void ApplyShapeFromUI();	// UI → Player(判定はPlayer側)

	void ShowMessage(const std::string& _message);

private:
	Player* mpPlayer = nullptr;

	// [column][row] の順で保持 (Player::mPieces と同じ並び)
	PlayerPieceUI* mPieceUI
		[GameConfig::PLAYER_PIECE_SIZE]
		[GameConfig::PLAYER_PIECE_SIZE]{};

	UI* mpConfirmUI = nullptr;
	UI* mpBackUI = nullptr;
	UI* mpMessageUI = nullptr;	// 変形できない理由の表示用
};
