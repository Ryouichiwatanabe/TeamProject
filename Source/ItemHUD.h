#pragma once
#include "UI.h"
#include "GameConfig.h"
#include <array>

class ItemHUD : public UI {
public:
	explicit ItemHUD(int _stage) : mStage(_stage) {}
	void Init() override;
	void SetPicked(int _slot);

private:
	static constexpr int   LAYER_HUD = 1;		// PauseUIの黒オーバーレイ(0)より上
	static constexpr float START_X = 1080.0f;
	static constexpr float START_Y = 50.0f;
	static constexpr float SPACING = 80.0f;
	static constexpr float ICON_SCALE = 0.8f;

	int mStage;
	std::array<UI*, GameConfig::MAX_ITEMS_PER_STAGE> mpIcons{};
};