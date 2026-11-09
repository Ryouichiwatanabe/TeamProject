#include "StageBackGround.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EffectManager.h"

#include "EffectSteam.h"

#include "ScreenConfig.h"

#include <format>

namespace
{
	// ステージ専用の背景画像がないときに使う画像
	constexpr const char* DEFAULT_BG_PATH =
		"Resource/UI/BackGround/Stage0_bg.png";

	// 背景の明るさ（0:真っ暗 ～ 255:通常）
	constexpr int BG_WHITE_ALPHA = 80;
}

void StageBackGround::Init()
{
	const int stageNum = Master::mpSceneManager->GetStageNumber();

	// ステージ番号に応じた画像ファイルのロード
	LoadUIGraph(BuildBackgroundPath(stageNum));

	const VECTOR size = GetTransform().GetSize();
	const VECTOR scale = VGet(
		ScreenConfig::SCREEN_WIDTH / size.x,
		ScreenConfig::SCREEN_HEIGHT / size.y,
		0.0f
	);
	GetTransform().SetScale(scale);
	SetEnabled(true);

	Master::mpSceneManager
		->GetCurrentScene()
		->GetEffectManager()
		->Play<EffectSteam>(stageNum);
}

void StageBackGround::Update()
{
	UI::Update();
}

void StageBackGround::Draw()
{
	// UIManagerから呼ばれるが、ここで描くとステージより前に出てしまう
	// 実際の描画は DrawBackground() で行う
}

void StageBackGround::DrawBackground()
{
	if (!IsEnabled())
		return;	

	UI::Draw();

	// -- 白い半透明の四角を重ねて霞ませる -- //
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BG_WHITE_ALPHA);
	DrawBox(
		0, 0,
		static_cast<int>(ScreenConfig::SCREEN_WIDTH),
		static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
		GetColor(255, 255, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

std::string StageBackGround::BuildBackgroundPath(int _stageNumber) const
{
	return std::format("Resource/UI/BackGround/Stage{}_bg.png", _stageNumber);
}