#pragma once

#include "Scene.h"
#include "FloatingMotion.h"

class TitleScene : public Scene
{
public:
	TitleScene();
	virtual ~TitleScene() = default;
	void Initialize() override;
	void Update(float _deltaTime) override;
	void Draw() override;
	void Finalize() override;
private:
	bool mbStartSelected = false;
	bool mbPreviousMouseLeft = false;

private:
	int mBgGroundHandle; //背景画像ハンドル
	int mTitleLogoHandle;//タイトルロゴ画像ハンドル
	int mStartTofuHandle = -1;
private:
	FloatingMotion mTitleFloatingMotion;

};