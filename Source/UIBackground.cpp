#include "UIBackground.h"
#include "ScreenConfig.h"
#include <format>
#include <filesystem>
#include <DxLib.h>

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EffectSteam.h"

namespace
{
	// ステージ専用の背景画像がないときに使う画像
	constexpr const char* DEFAULT_BG_PATH =
		"Resource/StageSelect/StageSelectMaterial/Foundation.png";

	// 背景の明るさ（0:真っ暗 ～ 255:通常）
	constexpr int BG_BRIGHTNESS = 160;
}

UIBackground::~UIBackground()
{
	if (mGraphHandle != -1)
	{
		DeleteGraph(mGraphHandle);
		mGraphHandle = -1;
	}
}

void UIBackground::Init()
{
	SetEnabled(true);
}

void UIBackground::Update()
{
	UI::Update();
}

void UIBackground::Draw()
{
	// UIManagerから呼ばれるが、ここで描くとステージより前に出てしまう
	// 実際の描画は DrawBackground() で行う
}

void UIBackground::DrawBackground()
{
	if (!IsEnabled() || mGraphHandle == -1)
	{
		return;
	}

	// 背景を少し暗くして、手前のブロックやキャラを目立たせる
	SetDrawBright(BG_BRIGHTNESS, BG_BRIGHTNESS, BG_BRIGHTNESS);

	DrawExtendGraph(
		0,
		0,
		static_cast<int>(ScreenConfig::SCREEN_WIDTH),
		static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
		mGraphHandle,
		TRUE
	);

	// 他の描画に影響しないよう必ず戻す
	SetDrawBright(255, 255, 255);
}

void UIBackground::SetStageByNumber(int _stageNumber)
{
	if (mnCurrentStageNumber == _stageNumber)
	{
		return;
	}

	mnCurrentStageNumber = _stageNumber;

	// ★ステージ番号を渡して、そのステージ専用の位置・範囲で湯気エフェクトを生成！
	Master::mpSceneManager
		->GetCurrentScene()
		->GetEffectManager()
		->Play<EffectSteam>(_stageNumber);

	const std::string bgFilePath = BuildBackgroundPath(mnCurrentStageNumber);

	// 該当ステージの画像があればロード、なければデフォルト画像
	if (std::filesystem::exists(bgFilePath))
	{
		LoadBackgroundGraph(bgFilePath);
	}
	else
	{
		OutputDebugStringA(
			("背景画像が見つかりません: " + bgFilePath + "\n").c_str()
		);

		if (std::filesystem::exists(DEFAULT_BG_PATH))
		{
			LoadBackgroundGraph(DEFAULT_BG_PATH);
		}
	}
}

void UIBackground::LoadBackgroundGraph(const std::string& _path)
{
	// 既に別の画像があれば先に解放
	if (mGraphHandle != -1)
	{
		DeleteGraph(mGraphHandle);
		mGraphHandle = -1;
	}

	mGraphHandle = LoadGraph(_path.c_str());

	if (mGraphHandle == -1)
	{
		OutputDebugStringA(
			("背景画像のロードに失敗: " + _path + "\n").c_str()
		);
	}
}

std::string UIBackground::BuildBackgroundPath(int _stageNumber) const
{
	return std::format("Resource/UI/BackGround/Stage{}_bg.png", _stageNumber);
}