#pragma once
#include "UI.h"

/* memo :
* 「(画像) リスタート！！」を表示し、長押し中は画像の周りに残り時間のリングを描くUI。
* 押し切ったかどうかを返すだけで、Playerは知らない（リスポーンの実行はGameScene側）。
*/
class RespawnUI : public UI
{
public:
	~RespawnUI() override;

	void Init() override;
	void Draw() override;

	// 長押しの進み具合を更新する。押し切ったフレームだけ true を返す
	bool UpdateHold(bool _isHeld, float _deltaTime);

private:
	float GetProgress() const { return mfHoldTimer / HOLD_TIME; }	// 0.0〜1.0

	void CreateRingGraph();		// 白いリング画像を作る(Initで1回)

private:
	static constexpr float HOLD_TIME = 1.0f;		// 何秒押せば発動するか

	// -- 大きさ -- //
	static constexpr float ICON_SCALE = 1.6f;		// 50px → 80px
	static constexpr int   RING_RADIUS = 60;		// 画像(80×80)の外側を囲む半径
	static constexpr int   RING_THICKNESS = 10;
	static constexpr float TEXT_MARGIN = 22.0f;		// リングとテキストの間隔

	int mnRingGraph = -1;			// リング画像のハンドル

	float mfHoldTimer = 0.0f;		// 押し続けている時間(秒)
	bool mbWaitRelease = false;		// 発動後、一度キーを離すまで再発動しない
};