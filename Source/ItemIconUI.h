#pragma once
#include "UI.h"

// アイコン1つ分。未取得のときは画像だけ暗く描く(テキストは暗くしない)
class ItemIconUI : public UI {
public:
	void Draw() override;
	void SetPicked(bool _picked) { mbPicked = _picked; }
	void SetShadowBright(int _bright) { mnShadowBright = _bright; }	// 0 = 真っ黒(シルエット)

private:
	static constexpr int DEFAULT_SHADOW_BRIGHT = 80;	// 未取得時の明るさ（0 = 真っ黒, 255 = 通常）

	bool mbPicked = false;
	int mnShadowBright = DEFAULT_SHADOW_BRIGHT;
};