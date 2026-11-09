#pragma once

#include "Scene.h"
#include "UIHoverScaler.h"

class UI;

class TitleScene : public Scene
{
public:
	TitleScene() = default;
	~TitleScene() = default;

	void Initialize() override;
	void Update(float _deltaTime) override;
	void Draw() override;
	void Finalize() override;
private:
	bool mbStartSelected = false;
	bool mbPreviousMouseLeft = false;

private:
	UI* mpBackGround = nullptr;	// 背景画像
	UI* mpLogo = nullptr;		// ロゴ画像
	UI* mpStartButton = nullptr;
	UI* mpQuitButton = nullptr;
};