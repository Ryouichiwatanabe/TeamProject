#pragma once
#include "UI.h"

class RoundedImageUI;
class ProgressBarUI;
class SteamUI;
class BlinkTextUI;

/* memo :
* ロード画面（LoadScene）のUIをまとめる親クラス。
* ClearUI / PauseUI と同じく、Init() で子UIを生成して自分にぶら下げる。
* 時間の進み具合は LoadScene から SetLoadState() で受け取る。
*
* // -- 画面構成（上から） -- //
*	中央パネル : ステージ名・湯気・進行状況・出汁ゲージ
*	まな板     : ステージごとの添付画像（角丸）
*	Tipsの板   : お料理メモの見出しと本文
*/
class LoadUI : public UI
{
public:
	void Init() override;

	void SetLoadState(float _elapsed, float _progress, bool _ready);	// 毎フレーム LoadScene から呼ぶ
	void Release();	// 自前で作った画像の解放（LoadScene::Finalize で呼ぶ）

private:
	template<class T>
	T* CreateChild(int _layer, float _centerX, float _centerY, float _width = 0.0f, float _height = 0.0f);
	UI* CreateText(float _centerY, FontId _fontId, unsigned int _color, const char* _text);

	void CreateBackground();
	void CreateLoadPanel(int _stageNumber);
	void CreateTipImage(const char* _imageFile);
	void CreateTipPlate(const char* _title, const char* _content);

private:
	// -- 描画順（小さいほど奥） -- //
	static constexpr int LAYER_BACKGROUND = 0;
	static constexpr int LAYER_DECORATION = 1;
	static constexpr int LAYER_SHADOW = 2;
	static constexpr int LAYER_BASE = 3;		// パネル・まな板・Tipsの板
	static constexpr int LAYER_CONTENT = 4;		// 文字・画像・ゲージ

	// -- 中央パネル -- //
	static constexpr float PANEL_W = 820.0f;
	static constexpr float PANEL_H = 310.0f;
	static constexpr float PANEL_TOP = 64.0f;

	// -- 添付画像（まな板）の置き場 -- //
	static constexpr float IMAGE_AREA_TOP = 392.0f;
	static constexpr float IMAGE_AREA_BOTTOM = 848.0f;
	static constexpr float BOARD_PADDING = 24.0f;		// 画像の周りに見せるまな板の幅
	static constexpr float BOARD_HANDLE_W = 44.0f;		// まな板右端の持ち手の穴の分
	static constexpr float IMAGE_MAX_W = 1100.0f;
	static constexpr float IMAGE_MAX_SCALE = 3.0f;		// 小さい画像を伸ばしすぎない上限
	static constexpr float IMAGE_CORNER_RADIUS = 18.0f;	// 画像の角の丸み(画面上のpx)

	// -- Tipsの板 -- //
	static constexpr float PLATE_W = 1300.0f;
	static constexpr float PLATE_H = 150.0f;
	static constexpr float PLATE_TOP = 872.0f;

private:
	UI* mpStatusText = nullptr;				// したごしらえ中... / 完了！
	BlinkTextUI* mpClickText = nullptr;		// 画面をクリックしてスタート
	ProgressBarUI* mpProgressBar = nullptr;
	SteamUI* mpSteam = nullptr;
	RoundedImageUI* mpTipImage = nullptr;	// 添付画像（無いステージは nullptr）
};
