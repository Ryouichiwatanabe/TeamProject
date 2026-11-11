#include "LoadUIParts.h"

#include "Master.h"
#include "ResourceManager.h"

#include <DxLib.h>
#include <algorithm>
#include <cmath>

namespace
{
	struct Rect
	{
		int left, top, right, bottom;
	};

	// UITransform の中心と大きさから、画面上の四角形を求める
	Rect GetRect(UI& _ui)
	{
		auto& transform = _ui.GetTransform();
		const VECTOR center = transform.GetCenterPosition();
		const VECTOR size = transform.GetSize();

		return {
			static_cast<int>(center.x - size.x / 2.0f),
			static_cast<int>(center.y - size.y / 2.0f),
			static_cast<int>(center.x + size.x / 2.0f),
			static_cast<int>(center.y + size.y / 2.0f)
		};
	}

	// 拡縮をなめらかにする描画モードに一時的に切り替える
	class ScopedBilinear
	{
	public:
		ScopedBilinear() : mnPrevMode(GetDrawMode()) { SetDrawMode(DX_DRAWMODE_BILINEAR); }
		~ScopedBilinear() { SetDrawMode(mnPrevMode); }

	private:
		int mnPrevMode;
	};
}

// ======================================================================
// RoundRectUI
// ======================================================================
void RoundRectUI::Draw()
{
	const Rect r = GetRect(*this);

	if (mnAlpha < 255)
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, mnAlpha);

	if (mnRadius <= 0)
	{
		DrawBox(r.left, r.top, r.right, r.bottom, mnFillColor, TRUE);
		if (mbOutline)
			DrawBox(r.left, r.top, r.right, r.bottom, mnOutlineColor, FALSE);
	}
	else
	{
		DrawRoundRect(r.left, r.top, r.right, r.bottom, mnRadius, mnRadius, mnFillColor, TRUE);
		if (mbOutline)
			DrawRoundRect(r.left, r.top, r.right, r.bottom, mnRadius, mnRadius, mnOutlineColor, FALSE);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	GetText().Draw();
}

// ======================================================================
// NineSliceUI
// ======================================================================
void NineSliceUI::SetGraph(const std::string& _filename)
{
	mnGraphHandle = Master::mpResource->LoadGraphics(_filename);
}

void NineSliceUI::SetBorder(int _left, int _top, int _right, int _bottom)
{
	mnBorderLeft = _left;
	mnBorderTop = _top;
	mnBorderRight = _right;
	mnBorderBottom = _bottom;
}

void NineSliceUI::Draw()
{
	if (mnGraphHandle == -1)
		return;

	int w, h;
	GetGraphSize(mnGraphHandle, &w, &h);
	const Rect r = GetRect(*this);

	// 切れ目の座標（画像側と画面側）。端の幅は等倍のまま、真ん中だけ伸ばす
	const int sx[4] = { 0, mnBorderLeft, w - mnBorderRight, w };
	const int sy[4] = { 0, mnBorderTop, h - mnBorderBottom, h };
	const int dx[4] = { r.left, r.left + mnBorderLeft, r.right - mnBorderRight, r.right };
	const int dy[4] = { r.top, r.top + mnBorderTop, r.bottom - mnBorderBottom, r.bottom };

	ScopedBilinear bilinear;
	for (int iy = 0; iy < 3; ++iy)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			DrawRectExtendGraph(
				dx[ix], dy[iy], dx[ix + 1], dy[iy + 1],
				sx[ix], sy[iy], sx[ix + 1] - sx[ix], sy[iy + 1] - sy[iy],
				mnGraphHandle, TRUE);
		}
	}

	GetText().Draw();
}

// ======================================================================
// RoundedImageUI
// ======================================================================
bool RoundedImageUI::Setup(const std::string& _filename, float _targetArea, float _maxWidth, float _maxHeight, float _maxScale, float _cornerRadius)
{
	Release();

	const int srcHandle = Master::mpResource->LoadGraphics(_filename);
	if (srcHandle == -1)
		return false;

	int w, h;
	GetGraphSize(srcHandle, &w, &h);

	// 1. 面積をそろえる倍率：画面上の面積が _targetArea になるようにする
	//    （面積は倍率の2乗で変わるので、面積比の平方根が倍率になる）
	const float areaScale = std::sqrt(_targetArea / (static_cast<float>(w) * h));

	// 2. ただし範囲からはみ出さない・伸ばしすぎないよう、上限で抑える
	mfScale = (std::min)({ areaScale, _maxWidth / w, _maxHeight / h, _maxScale });
	GetTransform().SetSize(VGet(w * mfScale, h * mfScale, 0.0f));

	// 画面上で _cornerRadius px に見えるよう、元画像側の半径に直してから切り抜く
	const int radiusOnSource = static_cast<int>(_cornerRadius / mfScale);
	const int rounded = CreateRoundedGraph(srcHandle, radiusOnSource);

	if (rounded != -1)
	{
		mnGraphHandle = rounded;
		mbOwnsHandle = true;
	}
	else
	{
		// 切り抜きに失敗したら、角はそのままで元画像を使う
		mnGraphHandle = srcHandle;
		mbOwnsHandle = false;
	}
	return true;
}

int RoundedImageUI::CreateRoundedGraph(int _srcHandle, int _radius)
{
	int w, h;
	GetGraphSize(_srcHandle, &w, &h);

	const int mask = MakeScreen(w, h, TRUE);	// 透明度の型抜き用
	const int dest = MakeScreen(w, h, TRUE);	// 完成した角丸画像
	if (mask == -1 || dest == -1)
	{
		if (mask != -1) DeleteGraph(mask);
		if (dest != -1) DeleteGraph(dest);
		return -1;
	}

	const int prevScreen = GetDrawScreen();

	// 1. 型抜き：透明な画面に、角丸の四角を不透明で塗る
	SetDrawScreen(mask);
	ClearDrawScreen();
	DrawRoundRect(0, 0, w, h, _radius, _radius, GetColor(255, 255, 255), TRUE);

	// 2. 合成：色(RGB)は元画像から、透明度(A)は型抜きから取って dest に書き込む
	GraphBlendBlt(
		_srcHandle, mask, dest, 255,
		DX_GRAPH_BLEND_RGBA_SELECT_MIX,
		DX_RGBA_SELECT_SRC_R, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_B,
		DX_RGBA_SELECT_BLEND_A);

	SetDrawScreen(prevScreen);
	DeleteGraph(mask);

	return dest;
}

void RoundedImageUI::Release()
{
	if (mbOwnsHandle && mnGraphHandle != -1)
		DeleteGraph(mnGraphHandle);

	mnGraphHandle = -1;
	mbOwnsHandle = false;
}

void RoundedImageUI::Draw()
{
	if (mnGraphHandle == -1)
		return;

	const VECTOR center = GetTransform().GetCenterPosition();

	ScopedBilinear bilinear;
	DrawRotaGraphF(center.x, center.y, mfScale, 0.0, mnGraphHandle, TRUE);
}

// ======================================================================
// ProgressBarUI
// ======================================================================
void ProgressBarUI::Draw()
{
	const Rect r = GetRect(*this);
	const int radius = (r.bottom - r.top) / 2;

	// ゲージの背景（薄いベージュ）
	DrawRoundRect(r.left, r.top, r.right, r.bottom, radius, radius, GetColor(220, 210, 195), TRUE);

	// 進行部分（出汁オレンジ）
	const float progress = (std::clamp)(mfProgress, 0.0f, 1.0f);
	const int fillW = static_cast<int>((r.right - r.left) * progress);
	if (fillW > 0)
		DrawRoundRect(r.left, r.top, r.left + fillW, r.bottom, radius, radius, GetColor(230, 140, 50), TRUE);

	// 縁取り
	DrawRoundRect(r.left, r.top, r.right, r.bottom, radius, radius, GetColor(95, 75, 60), FALSE);
}

// ======================================================================
// SteamUI
// ======================================================================
void SteamUI::Draw()
{
	const VECTOR center = GetTransform().GetCenterPosition();

	for (int i = 0; i < 3; ++i)
	{
		// 0→1 を繰り返す値。3つの湯気は少しずつタイミングをずらす
		const float t = std::fmod(mfTime * 1.6f + i * 0.33f, 1.0f);

		const float x = center.x + (i - 1) * 38.0f + std::sin(t * 6.28f + i) * 12.0f;
		const float y = center.y - t * 50.0f;
		const int alpha = static_cast<int>(std::sin(t * 3.14159f) * 170);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawCircleAA(x, y, 14.0f * (0.8f + t * 0.5f), 16, GetColor(210, 190, 170), TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

// ======================================================================
// BlinkTextUI
// ======================================================================
void BlinkTextUI::Draw()
{
	const float blink = (std::sin(mfTime * 6.0f) + 1.0f) * 0.5f;	// 0～1 を往復
	const int alpha = static_cast<int>(120 + 135 * blink);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	GetText().Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
