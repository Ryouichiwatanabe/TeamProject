#pragma once
#include <DxLib.h>

// 全ステージに馴染むシンプル＆洗練されたキッチンフレーム（額縁）
class KitchenFrame
{
public:
	KitchenFrame() = default;
	~KitchenFrame() = default;

	void Draw() const;

private:
	void DrawMinimalFrame() const;
	void DrawInnerShadow() const;
	void DrawCornerRivets() const;
};