#include "ItemIconUI.h"
#include <DxLib.h>

void ItemIconUI::Draw() {
	if (mbPicked) {
		UI::Draw();		// 取得済み：普通に描く
		return;
	}

	// 今の明るさを覚えておく（他の誰かが変えていても壊さないため）
	int r, g, b;
	GetDrawBright(&r, &g, &b);

	SetDrawBright(mnShadowBright, mnShadowBright, mnShadowBright);
	GetGraphic().Draw();	// 画像だけ暗くする
	SetDrawBright(r, g, b);	// 必ず元に戻す

	GetText().Draw();		// 「?」などの文字は通常の明るさで描く
}