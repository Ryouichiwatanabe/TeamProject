#include "UIBackground.h"
#include "Stage.h"
#include "ScreenConfig.h"
#include <format>
#include <filesystem>
#include <DxLib.h>

UIBackground::~UIBackground()
{
	// シーン破棄時などに画像をメモリから解放
	if (mGraphHandle != -1)
	{
		DeleteGraph(mGraphHandle);
		mGraphHandle = -1;
	}
}

void UIBackground::Init()
{
	SetLayer(0);
	SetEnabled(true);
}

void UIBackground::Update()
{
	UI::Update();
}

void UIBackground::Draw()
{
	if (!IsEnabled())
	{
		return;
	}

	if (mGraphHandle != -1)
	{
		// 1. 描画輝度を下げる（0: 完全な真っ黒 ? 255: 通常の明るさ）
		// 例: 160?180 程度にすると、手前のブロックやキャラが引き立つ適度な薄暗さになります
		SetDrawBright(160, 160, 160);

		// 2. 背景の描画
		DrawExtendGraph(
			0,
			0,
			static_cast<int>(ScreenConfig::SCREEN_WIDTH),
			static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
			mGraphHandle,
			TRUE
		);

		// 3. ★重要: 他のオブジェクト描画に影響が出ないよう、必ず標準の明るさ（255）に戻す
		SetDrawBright(255, 255, 255);
	}
}

void UIBackground::SetStage(const Stage* _stage)
{
	if (!_stage)
	{
		return;
	}

	//SetStageByNumber(_stage->GetStageNumber());
}

void UIBackground::SetStage(const Stage& _stage)
{
	//SetStageByNumber(_stage.GetStageNumber());
}

void UIBackground::SetStageByNumber(int _stageNumber)
{
	if (mnCurrentStageNumber == _stageNumber)
	{
		return;
	}

	mnCurrentStageNumber = _stageNumber;

	const std::string bgFilePath = BuildBackgroundPath(mnCurrentStageNumber);

	// 該当ステージ画像があればロード、なければフォールバック
	if (std::filesystem::exists(bgFilePath))
	{
		LoadBackgroundGraph(bgFilePath);
	}
	else
	{
		const std::string defaultPath = "Resource/Stage/Foundatin.png";
		if (std::filesystem::exists(defaultPath))
		{
			LoadBackgroundGraph(defaultPath);
		}
	}
}

void UIBackground::LoadBackgroundGraph(const std::string& _path)
{
	// 既に別の画像が読み込まれていた場合は先に解放する
	if (mGraphHandle != -1)
	{
		DeleteGraph(mGraphHandle);
		mGraphHandle = -1;
	}

	// 新しい背景画像をロード
	mGraphHandle = LoadGraph(_path.c_str());
}

std::string UIBackground::BuildBackgroundPath(int _stageNumber) const
{
	return std::format("Resource/StageGraphic/Stage{}_bg.png", _stageNumber);
}