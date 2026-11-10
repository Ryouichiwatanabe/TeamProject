#include "StageBackGround.h"
#include "StageBackGroundTable.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EffectManager.h"
#include "ResourceManager.h"

#include "EffectSteam.h"

#include "ScreenConfig.h"
#include "GameConfig.h"

#include <DxLib.h>
#include <format>

namespace
{
	// 背景の明るさ（0:真っ暗 ～ 255:通常）
	constexpr int BG_WHITE_ALPHA = 80;

	// 取得状態に対して、このレイヤーを描くか
	bool IsLayerVisible(const BgLayerDef& _layer, int _pickedBits)
	{
		const bool hasAllRequired = (_pickedBits & _layer.require) == _layer.require;
		const bool hasNoExcluded = (_pickedBits & _layer.exclude) == 0;
		return hasAllRequired && hasNoExcluded;
	}
}

void StageBackGround::Init()
{
	int stageNum = Master::mpSceneManager->GetStageNumber();
	if (Master::mpSceneManager->GetCurrentSceneType() != SCENE_TYPE::GAME_SCENE)
		stageNum = 0;

	mpDish = StageBackGroundTable::Find(stageNum);
	if (mpDish != nullptr)
		LoadDishGraphs();
	else
		LoadSingleGraph(stageNum);

	SetEnabled(true);

	Master::mpSceneManager
		->GetCurrentScene()
		->GetEffectManager()
		->Play<EffectSteam>(stageNum);
}

void StageBackGround::LoadSingleGraph(int _stageNumber)
{
	LoadUIGraph(BuildBackgroundPath(_stageNumber));

	const VECTOR size = GetTransform().GetSize();
	GetTransform().SetScale(VGet(
		ScreenConfig::SCREEN_WIDTH / size.x,
		ScreenConfig::SCREEN_HEIGHT / size.y,
		0.0f));
}

void StageBackGround::LoadDishGraphs()
{
	// -- 土台 -- //
	mnFoundationHandle = Master::mpResource->LoadGraphics(
		StageBackGroundTable::GetFilePath(*mpDish, mpDish->foundation));

	int width = 0;
	int height = 0;
	GetGraphSize(mnFoundationHandle, &width, &height);
	mfBaseWidth = (width > 0) ? static_cast<float>(width) : 1.0f;
	mfBaseHeight = (height > 0) ? static_cast<float>(height) : 1.0f;

	// -- 重ねる画像（テーブルと同じ順番で持つ） -- //
	mLayerHandles.clear();
	mLayerHandles.reserve(mpDish->layers.size());
	for (const auto& layer : mpDish->layers)
	{
		mLayerHandles.push_back(Master::mpResource->LoadGraphics(
			StageBackGroundTable::GetFilePath(*mpDish, layer.file)));
	}
}

void StageBackGround::SetPicked(int _slot)
{
	if (_slot < 0 || _slot >= GameConfig::MAX_ITEMS_PER_STAGE)
		return;
	mnPickedBits |= (1 << _slot);
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

	if (mpDish != nullptr)
		DrawDish();
	else
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

void StageBackGround::DrawDish() const
{
	// 土台：Foundationの中心に等倍で置く → 画面いっぱいになる
	DrawImage(mnFoundationHandle, mfBaseWidth * 0.5f, mfBaseHeight * 0.5f, 1.0f, 0.0f);

	for (size_t i = 0; i < mpDish->layers.size(); ++i)
	{
		const BgLayerDef& layer = mpDish->layers[i];
		if (!IsLayerVisible(layer, mnPickedBits))
			continue;

		DrawImage(mLayerHandles[i], layer.x, layer.y, layer.scale, layer.angle);
	}
}

// Foundation画像のピクセル座標 → 画面座標 に変換して描く
void StageBackGround::DrawImage(int _handle, float _x, float _y, float _scale, float _angle) const
{
	if (_handle == -1)
		return;

	int width = 0;
	int height = 0;
	GetGraphSize(_handle, &width, &height);

	const float ratioX = ScreenConfig::SCREEN_WIDTH / mfBaseWidth;
	const float ratioY = ScreenConfig::SCREEN_HEIGHT / mfBaseHeight;

	DrawRotaGraph3F(
		_x * ratioX, _y * ratioY,
		width * 0.5f, height * 0.5f,
		_scale * ratioX, _scale * ratioY,
		_angle, _handle, TRUE, FALSE);
}

std::string StageBackGround::BuildBackgroundPath(int _stageNumber) const
{
	return std::format("Resource/UI/BackGround/Stage{}_bg.png", _stageNumber);
}