#pragma once
#include "UI.h"
#include <string>

/* memo :
* LoadScene（ロード画面）専用の描画部品。
* どれも UI を継承して Draw() だけを上書きし、独自の描き方をする。
*
* // -- 共通ルール -- //
*	位置 : UITransform の「アンカー + 座標」（中心点）
*	大きさ : UITransform の SetSize()（描画サイズ）
*/

// 角丸の四角形（背景・影・Tipsの板など）
class RoundRectUI : public UI
{
public:
	void Draw() override;

	void SetFillColor(unsigned int _color) { mnFillColor = _color; }
	void SetOutlineColor(unsigned int _color) { mnOutlineColor = _color; mbOutline = true; }
	void SetRadius(int _radius) { mnRadius = _radius; }		// 0 なら角なしの四角
	void SetAlpha(int _alpha) { mnAlpha = _alpha; }			// 0～255（影を薄くするときに使う）

private:
	unsigned int mnFillColor = GetColor(255, 255, 255);
	unsigned int mnOutlineColor = GetColor(0, 0, 0);
	bool mbOutline = false;
	int mnRadius = 0;
	int mnAlpha = 255;
};

// 画像を9分割して、角をつぶさずに好きな大きさへ伸ばすUI（ナインスライス）
class NineSliceUI : public UI
{
public:
	void Draw() override;

	void SetGraph(const std::string& _filename);
	void SetBorder(int _left, int _top, int _right, int _bottom);	// 伸ばさない端の幅(px)

private:
	int mnGraphHandle = -1;	// ResourceManager所有（ここでは解放しない）
	int mnBorderLeft = 16;
	int mnBorderTop = 16;
	int mnBorderRight = 16;
	int mnBorderBottom = 16;
};

// 角を丸く切り抜いた画像を、指定の範囲に収まるように拡縮して描くUI
class RoundedImageUI : public UI
{
public:
	void Draw() override;

	// 画像を読み込み、面積が _targetArea くらいになる倍率（ただし最大範囲・最大倍率は超えない）で角丸画像を作る（失敗時 false）
	bool Setup(const std::string& _filename, float _targetArea, float _maxWidth, float _maxHeight, float _maxScale, float _cornerRadius);
	void Release();	// 自前で作った角丸画像を解放する（シーン終了時に必ず呼ぶ）

private:
	static int CreateRoundedGraph(int _srcHandle, int _radius);	// 角丸に切り抜いた画像を作る

private:
	int mnGraphHandle = -1;		// 描画に使う画像
	bool mbOwnsHandle = false;	// true : 自前で作った画像なので Release() で解放する
	float mfScale = 1.0f;		// 描画倍率
};

// 出汁のプログレスバー
class ProgressBarUI : public UI
{
public:
	void Draw() override;

	void SetProgress(float _progress) { mfProgress = _progress; }	// 0.0～1.0

private:
	float mfProgress = 0.0f;
};

// お鍋の湯気アニメーション（3つの円がゆらゆら昇る）
class SteamUI : public UI
{
public:
	void Draw() override;

	void SetTime(float _time) { mfTime = _time; }

private:
	float mfTime = 0.0f;
};

// ふわふわ点滅するテキスト
class BlinkTextUI : public UI
{
public:
	void Draw() override;

	void SetTime(float _time) { mfTime = _time; }

private:
	float mfTime = 0.0f;
};
