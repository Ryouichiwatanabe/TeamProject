#include "StageSelectScene_ex.h"

#include "UI.h"
#include "UIManager.h"
#include "ItemIconUI.h"

#include "ItemTable.h"
#include "ItemRecord.h"

#include "Master.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "SoundManager.h"

#include "ScreenConfig.h"

#include <algorithm>	// std::min

namespace
{
	// -- 画像パス -- //
	constexpr const char* BACKGROUND_PATH = "Resource/UI/Scene_StageSelect/notFoundation.png";
	constexpr const char* DISH_PANEL_PATH = "Resource/UI/Scene_StageSelect/DeshPanel.png";
	constexpr const char* ITEM_PANEL_PATH = "Resource/UI/Scene_StageSelect/ItemPanel.png";

	// -- シークレット表示 -- //
	constexpr const char* SECRET_NAME = "???";
	constexpr const char* SECRET_DISH = "?";

	// -- レイヤー -- //
	constexpr int LAYER_BACKGROUND = 0;
	constexpr int LAYER_CARD = 1;		// DeshPanel(灰色の枠ごと覆う)
	constexpr int LAYER_PANEL = 2;		// ItemPanel
	constexpr int LAYER_CONTENT = 3;	// 料理名・料理画像・アイテム画像

	// -- レイアウト(すべて背景画像上のピクセル座標) -- //
	// 灰色の枠の中心。並びは AllClear.png に合わせる(左ページ 1,2 / 3,4、右ページ 5,6 / 7,8)
	constexpr float STAGE_CENTERS[GameConfig::STAGE_COUNT][2] = {
		{ 242.0f, 206.5f }, { 384.5f, 206.5f },	// Stage1, 2
		{ 242.0f, 410.5f }, { 389.5f, 410.5f },	// Stage3, 4
		{ 545.5f, 206.5f }, { 688.0f, 206.5f },	// Stage5, 6
		{ 546.0f, 410.5f }, { 690.0f, 410.5f },	// Stage7, 8
	};

	constexpr float CARD_WIDTH = 121.0f;	// 灰色の枠(117×174)を完全に隠すため少し大きめ
	constexpr float CARD_HEIGHT = 178.0f;

	constexpr float NAME_OFFSET_Y = -63.0f;	// 枠の中心からのずれ
	constexpr float DISH_OFFSET_Y = -3.0f;
	constexpr float ITEM_OFFSET_Y = 61.0f;
	constexpr float ITEM_SPACING = 37.0f;	// アイテム枠どうしの間隔

	constexpr float DISH_IMAGE_WIDTH = 100.0f;	// 料理画像を収める大きさ
	constexpr float DISH_IMAGE_HEIGHT = 75.0f;

	constexpr float ITEM_PANEL_SIZE = 32.0f;
	constexpr float ITEM_IMAGE_SIZE = 26.0f;
}

void StageSelectScene_ex::Initialize()
{
	CreateBackground();

	for (int stage = 1; stage <= GameConfig::STAGE_COUNT; ++stage)
		CreateStageCard(stage);
}

void StageSelectScene_ex::Update(float _deltaTime)
{
	for (int i = 0; i < GameConfig::STAGE_COUNT; ++i)
	{
		UI* panel = mpDishPanels[i];
		if (panel == nullptr || !panel->GetButton().IsClicked())
			continue;

		// Confirm を消費し、共通のクリックSE判定をスキップ
		InputManager::GetInstance().ConsumeButton(Button::Confirm);
		SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);

		Master::mpSceneManager->RequestGameScene(i + 1);
		break;
	}

	Scene::Update(_deltaTime);
}

void StageSelectScene_ex::Draw()
{
	Scene::Draw();
}

void StageSelectScene_ex::Finalize()
{
	// タイトルBGMを停止(旧StageSelectSceneと同じ)
	SoundManager::GetInstance()->StopBGM();

	mpBackground = nullptr;
	mpDishPanels.fill(nullptr);

	Scene::Finalize();
}

// ===================================================== //
//  生成
// ===================================================== //

void StageSelectScene_ex::CreateBackground()
{
	mpBackground = GetUIManager()->CreateUI<UI>();
	mpBackground->LoadUIGraph(BACKGROUND_PATH);
	mpBackground->SetLayer(LAYER_BACKGROUND);

	// 画面いっぱいに引き伸ばす(この倍率を他の配置計算でも使う)
	const VECTOR size = mpBackground->GetTransform().GetSize();
	if (size.x > 0.0f && size.y > 0.0f)
	{
		mfBgScaleX = ScreenConfig::SCREEN_WIDTH / size.x;
		mfBgScaleY = ScreenConfig::SCREEN_HEIGHT / size.y;
	}
	mpBackground->GetTransform().SetScale(VGet(mfBgScaleX, mfBgScaleY, 0.0f));

	mpBackground->SetEnabled(true);
}

void StageSelectScene_ex::CreateStageCard(int _stage)
{
	const Point center = { STAGE_CENTERS[_stage - 1][0], STAGE_CENTERS[_stage - 1][1] };
	const bool isCompleted = ItemRecord::GetInstance().IsAllObtained(_stage);	// 3つ揃ったか

	CreateDish(_stage, center, isCompleted);
	CreateDishName(_stage, center, isCompleted);
	CreateItems(_stage, center);
}

void StageSelectScene_ex::CreateDishName(int _stage, Point _center, bool _isCompleted)
{
	UI* name = GetUIManager()->CreateUI<UI>();
	name->SetLayer(LAYER_CONTENT);
	name->GetTransform().SetPosition(ToUIPosition(_center.x, _center.y + NAME_OFFSET_Y));

	UIText& text = name->GetText();
	text.SetFontId(FontId::SubTitle);
	text.SetColor(GetColor(0, 0, 0));
	text.SetUnderline(true);	// AllClear / notClear と同じく下線付き
	text.SetText(_isCompleted ? ItemTable::GetDishName(_stage) : SECRET_NAME);

	name->SetEnabled(true);
}

void StageSelectScene_ex::CreateDish(int _stage, Point _center, bool _isCompleted)
{
	// -- 料理パネル(灰色の枠ごと被せる。クリック判定もこれで行う) -- //
	UI* panel = GetUIManager()->CreateUI<UI>();
	panel->LoadUIGraph(DISH_PANEL_PATH);
	PlaceImage(panel, _center.x, _center.y, LAYER_CARD);
	StretchImage(panel, CARD_WIDTH, CARD_HEIGHT);
	panel->SetEnabled(true);

	mpDishPanels[_stage - 1] = panel;

	// -- 料理画像(未完成なら真っ黒なシルエット + 「?」) -- //
	const std::string path = ItemTable::GetDishFilePath(_stage);
	if (path.empty())
		return;

	const float y = _center.y + DISH_OFFSET_Y;

	ItemIconUI* dish = GetUIManager()->CreateUI<ItemIconUI>();
	dish->LoadUIGraph(path);
	PlaceImage(dish, _center.x, y, LAYER_CONTENT);
	FitImage(dish, DISH_IMAGE_WIDTH, DISH_IMAGE_HEIGHT);

	dish->SetShadowBright(0);
	dish->SetPicked(_isCompleted);

	if (!_isCompleted)
	{
		UIText& text = dish->GetText();
		text.SetFontId(FontId::Button);
		text.SetColor(GetColor(255, 255, 255));
		text.SetText(SECRET_DISH);
	}

	dish->SetEnabled(true);
}

void StageSelectScene_ex::CreateItems(int _stage, Point _center)
{
	const float y = _center.y + ITEM_OFFSET_Y;

	// 真ん中の枠が中心に来るようにずらす（3個なら -1, 0, +1 倍）
	constexpr float CENTER_INDEX = (GameConfig::MAX_ITEMS_PER_STAGE - 1) / 2.0f;

	for (int slot = 0; slot < GameConfig::MAX_ITEMS_PER_STAGE; ++slot)
	{
		const float x = _center.x + (slot - CENTER_INDEX) * ITEM_SPACING;

		// -- アイテムパネル(常に表示) -- //
		UI* panel = GetUIManager()->CreateUI<UI>();
		panel->LoadUIGraph(ITEM_PANEL_PATH);
		PlaceImage(panel, x, y, LAYER_PANEL);
		FitImage(panel, ITEM_PANEL_SIZE, ITEM_PANEL_SIZE);
		panel->SetEnabled(true);

		// -- アイテム画像(取得済みのものだけ) -- //
		if (!ItemRecord::GetInstance().IsObtained(_stage, slot))
			continue;

		const std::string path = ItemTable::GetFilePath(_stage, slot);
		if (path.empty())
			continue;

		UI* item = GetUIManager()->CreateUI<UI>();
		item->LoadUIGraph(path);
		PlaceImage(item, x, y, LAYER_CONTENT);
		FitImage(item, ITEM_IMAGE_SIZE, ITEM_IMAGE_SIZE);
		item->SetEnabled(true);
	}
}

// ===================================================== //
//  配置の補助
// ===================================================== //

void StageSelectScene_ex::PlaceImage(UI* _ui, float _x, float _y, int _layer) const
{
	_ui->SetLayer(_layer);
	_ui->GetTransform().SetPosition(ToUIPosition(_x, _y));
}

void StageSelectScene_ex::FitImage(UI* _ui, float _width, float _height) const
{
	auto& transform = _ui->GetTransform();
	const VECTOR size = transform.GetSize();
	if (size.x <= 0.0f || size.y <= 0.0f)
		return;

	// 縦・横それぞれ「枠に収まる倍率」を出し、小さい方を両方に使う(= はみ出さない)
	const float scale = (std::min)(
		_width * mfBgScaleX / size.x,
		_height * mfBgScaleY / size.y
		);
	transform.SetScale(VGet(scale, scale, 0.0f));
}

void StageSelectScene_ex::StretchImage(UI* _ui, float _width, float _height) const
{
	auto& transform = _ui->GetTransform();
	const VECTOR size = transform.GetSize();
	if (size.x <= 0.0f || size.y <= 0.0f)
		return;

	transform.SetScale(VGet(
		_width * mfBgScaleX / size.x,
		_height * mfBgScaleY / size.y,
		0.0f
	));
}

VECTOR StageSelectScene_ex::ToUIPosition(float _x, float _y) const
{
	return VGet(
		_x * mfBgScaleX - ScreenConfig::SCREEN_WIDTH * 0.5f,
		_y * mfBgScaleY - ScreenConfig::SCREEN_HEIGHT * 0.5f,
		0.0f
	);
}