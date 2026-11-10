#include "RespawnUI.h"
#include <DxLib.h>

RespawnUI::~RespawnUI()
{
	if (mnRingGraph != -1)
		DeleteGraph(mnRingGraph);
}

void RespawnUI::Init()
{
	SetEnabled(true);	// UIの初期値はfalseなので有効にする
	SetLayer(3);

	// 画面右下あたり（テキストが右にはみ出さない位置）
	GetTransform().SetAnchor(VGet(0.75f, 0.9f, 0.0f));

	// 画像(UIGraphicがTransformの中心に、Scaleを掛けて描く)
	LoadUIGraph("Resource/UI/Respawn/P-Key.png");
	GetTransform().SetScale(VGet(ICON_SCALE, ICON_SCALE, 0.0f));

	// テキストは画像の右側に、左揃え・縦中央で置く
	GetText().SetFontId(FontId::HUD);
	GetText().SetText("リスタート！！");
	GetText().SetAlign(UITextAlign::LEFT);
	GetText().SetVerticalAlign(UITextVerticalAlign::CENTER);
	GetText().SetOffset(VGet(RING_RADIUS + TEXT_MARGIN, 0.0f, 0.0f));

	CreateRingGraph();
}

void RespawnUI::CreateRingGraph()
{
	const int size = (RING_RADIUS + RING_THICKNESS) * 2;	// リングが収まる正方形
	const float center = size * 0.5f;

	// 透過付きの作業用画像を作る
	mnRingGraph = MakeScreen(size, size, TRUE);

	// 描画先を一時的に作業用画像へ切り替える（あとで元に戻す）
	const int prevScreen = GetDrawScreen();
	SetDrawScreen(mnRingGraph);
	ClearDrawScreen();

	// 塗りつぶし無し(FALSE) + 太さ指定で、白いリングを1回で描く
	DrawCircleAA(center, center, static_cast<float>(RING_RADIUS), 128,
		GetColor(255, 255, 255), FALSE, static_cast<float>(RING_THICKNESS));

	SetDrawScreen(prevScreen);
}

bool RespawnUI::UpdateHold(bool _isHeld, float _deltaTime)
{
	// 離したらリセット
	if (!_isHeld)
	{
		mfHoldTimer = 0.0f;
		mbWaitRelease = false;
		return false;
	}

	// 発動直後に押しっぱなしのときは、もう一度は発動しない
	if (mbWaitRelease)
		return false;

	mfHoldTimer += _deltaTime;
	if (mfHoldTimer >= HOLD_TIME)
	{
		mfHoldTimer = 0.0f;
		mbWaitRelease = true;
		return true;	// 押し切った
	}
	return false;
}

void RespawnUI::Draw()
{
	UI::Draw();	// 画像 + 「リスタート！！」

	// 押していないときはリングを描かない
	if (mfHoldTimer <= 0.0f || mnRingGraph == -1)
		return;

	const VECTOR center = GetTransform().GetCenterPosition();
	const int x = static_cast<int>(center.x);
	const int y = static_cast<int>(center.y);
	const double progress = GetProgress() * 100.0;	// DrawCircleGaugeは%指定

	// 下地：黒・半透明で一周
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	SetDrawBright(0, 0, 0);
	DrawCircleGauge(x, y, 100.0, mnRingGraph, 0.0);

	// 残り時間：進んだ分だけ真上から時計回りに削れていく(progress% 〜 100%)
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetDrawBright(255, 200, 80);
	DrawCircleGauge(x, y, 100.0, mnRingGraph, progress);

	SetDrawBright(255, 255, 255);	// 他の描画に影響しないよう戻す
}