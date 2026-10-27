#pragma once
#include "UI.h"

/* memo :
* 変形UIの「内側」1マス分のUI。
* クリックで選択(=ブロックを置く)/非選択を切り替える。
* 基盤ブロックのマスは選択を外せない(変形中も動かないブロックのため)。
*/
class PlayerPieceUI : public UI
{
public:
	PlayerPieceUI() = default;
	~PlayerPieceUI() = default;

	void Init() override;

	void Update() override;
	void Draw() override;

	// -- 選択状態のアクセサ -- //
	bool IsSelected() const { return mbSelected; }
	void SetSelected(bool _selected) { mbSelected = _selected; }
	void ToggleSelected() { mbSelected = !mbSelected; }

	// -- 基盤ブロック状態のアクセサ -- //
	bool IsBase() const { return mbIsBase; }
	void SetBase(bool _isBase) { mbIsBase = _isBase; }

private:
	void DrawBaseFrame();	// 基盤マスの赤枠描画

private:
	bool mbSelected = false;
	bool mbIsBase = false;
};
