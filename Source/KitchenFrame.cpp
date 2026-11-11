#include "KitchenFrame.h"
#include "ScreenConfig.h"

namespace
{
	constexpr float SCREEN_W = ScreenConfig::SCREEN_WIDTH;   // 1920.0f
	constexpr float SCREEN_H = ScreenConfig::SCREEN_HEIGHT;  // 1080.0f

	// フレームの厚み（ゲームの邪魔をしないスリム設計）
	constexpr int FRAME_THICK_Y = 26; // 上下の枠の太さ(px)
	constexpr int FRAME_THICK_X = 18; // 左右の枠の太さ(px)

	// ステンレス＆清潔感のあるキッチンのニュートラルカラー
	constexpr unsigned int COL_STEEL_LIGHT = 0xFFE8EEF2; // ステンレスハイライト
	constexpr unsigned int COL_STEEL_MID = 0xFFCBD4DC; // ステンレス基本色
	constexpr unsigned int COL_STEEL_DARK = 0xFF8E9BA8; // ステンレスエッジ
	constexpr unsigned int COL_TILE_LINE = 0xFFB4C0CB; // タイル調スリットライン
}

void KitchenFrame::Draw() const
{
	SetUseZBufferFlag(FALSE);
	SetWriteZBufferFlag(FALSE);
	SetUseLighting(FALSE);

	// 1. 内側に落ちる繊細なソフトシャドウ（額縁の奥行き感）
	DrawInnerShadow();

	// 2. 外周のミニマルなステンレスフレーム
	DrawMinimalFrame();

	// 3. 四隅の小さな固定ビス（リベット）
	DrawCornerRivets();

	SetUseZBufferFlag(TRUE);
	SetWriteZBufferFlag(TRUE);
	SetUseLighting(TRUE);
}

void KitchenFrame::DrawInnerShadow() const
{
	const int sw = static_cast<int>(SCREEN_W);
	const int sh = static_cast<int>(SCREEN_H);
	constexpr int SHADOW_SIZE = 14;

	// ふんわりとした薄いインナーシャドウ
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
	DrawBox(FRAME_THICK_X, FRAME_THICK_Y, sw - FRAME_THICK_X, FRAME_THICK_Y + SHADOW_SIZE, GetColor(20, 25, 30), TRUE);
	DrawBox(FRAME_THICK_X, sh - FRAME_THICK_Y - SHADOW_SIZE, sw - FRAME_THICK_X, sh - FRAME_THICK_Y, GetColor(20, 25, 30), TRUE);
	DrawBox(FRAME_THICK_X, FRAME_THICK_Y, FRAME_THICK_X + SHADOW_SIZE, sh - FRAME_THICK_Y, GetColor(20, 25, 30), TRUE);
	DrawBox(sw - FRAME_THICK_X - SHADOW_SIZE, FRAME_THICK_Y, sw - FRAME_THICK_X, sh - FRAME_THICK_Y, GetColor(20, 25, 30), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void KitchenFrame::DrawMinimalFrame() const
{
	const int sw = static_cast<int>(SCREEN_W);
	const int sh = static_cast<int>(SCREEN_H);

	// -- 上枠（スリムなステンレスヘアライン天板） -- //
	DrawBox(0, 0, sw, FRAME_THICK_Y, COL_STEEL_MID, TRUE);
	DrawBox(0, 0, sw, 4, COL_STEEL_LIGHT, TRUE);
	DrawBox(0, FRAME_THICK_Y - 3, sw, FRAME_THICK_Y, COL_STEEL_DARK, TRUE);

	// -- 下枠（スリムなステンレス天板） -- //
	DrawBox(0, sh - FRAME_THICK_Y, sw, sh, COL_STEEL_MID, TRUE);
	DrawBox(0, sh - FRAME_THICK_Y, sw, sh - FRAME_THICK_Y + 3, COL_STEEL_LIGHT, TRUE);
	DrawBox(0, sh - 4, sw, sh, COL_STEEL_DARK, TRUE);

	// -- 左右枠 -- //
	DrawBox(0, 0, FRAME_THICK_X, sh, COL_STEEL_MID, TRUE);
	DrawBox(0, 0, 3, sh, COL_STEEL_LIGHT, TRUE);
	DrawBox(FRAME_THICK_X - 3, 0, FRAME_THICK_X, sh, COL_STEEL_DARK, TRUE);

	DrawBox(sw - FRAME_THICK_X, 0, sw, sh, COL_STEEL_MID, TRUE);
	DrawBox(sw - FRAME_THICK_X, 0, sw - FRAME_THICK_X + 3, sh, COL_STEEL_LIGHT, TRUE);
	DrawBox(sw - 3, 0, sw, sh, COL_STEEL_DARK, TRUE);

	// 内側の境界ライン（細くシャープな輪郭）
	DrawLine(FRAME_THICK_X, FRAME_THICK_Y, sw - FRAME_THICK_X, FRAME_THICK_Y, COL_STEEL_DARK, 2);
	DrawLine(FRAME_THICK_X, sh - FRAME_THICK_Y, sw - FRAME_THICK_X, sh - FRAME_THICK_Y, COL_STEEL_DARK, 2);
	DrawLine(FRAME_THICK_X, FRAME_THICK_Y, FRAME_THICK_X, sh - FRAME_THICK_Y, COL_STEEL_DARK, 2);
	DrawLine(sw - FRAME_THICK_X, FRAME_THICK_Y, sw - FRAME_THICK_X, sh - FRAME_THICK_Y, COL_STEEL_DARK, 2);

	// 上枠と下枠に等間隔に入る清潔なタイルスリット（目立ちすぎない細線）
	for (int x = 160; x < sw - 160; x += 160)
	{
		DrawLine(x, 4, x, FRAME_THICK_Y - 3, COL_TILE_LINE, 1);
		DrawLine(x, sh - FRAME_THICK_Y + 3, x, sh - 4, COL_TILE_LINE, 1);
	}
}

void KitchenFrame::DrawCornerRivets() const
{
	const int sw = static_cast<int>(SCREEN_W);
	const int sh = static_cast<int>(SCREEN_H);
	const int rx = FRAME_THICK_X / 2 + 1;
	const int ry = FRAME_THICK_Y / 2;

	const int points[4][2] = {
		{ rx, ry },
		{ sw - rx, ry },
		{ rx, sh - ry },
		{ sw - rx, sh - ry }
	};

	// 四隅の小さなフラットビス（直径6px）
	for (int i = 0; i < 4; ++i)
	{
		const int px = points[i][0];
		const int py = points[i][1];

		DrawCircle(px, py, 4, COL_STEEL_DARK, TRUE);
		DrawCircle(px, py, 3, COL_STEEL_LIGHT, TRUE);
		DrawLine(px - 2, py, px + 2, py, COL_STEEL_DARK, 1);
	}
}