#pragma once
#include "UI.h"

// ItemHUD のアイコン1つ分。未取得のときは暗く描く
class ItemIconUI : public UI {
public:
	void Draw() override;
	void SetPicked(bool _picked) { mbPicked = _picked; }

private:
	static constexpr int SHADOW_BRIGHT = 80;	// 未取得時の明るさ（0 = 真っ黒, 255 = 通常）

	bool mbPicked = false;
};