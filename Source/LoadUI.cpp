#include "LoadUI.h"
#include "LoadUIParts.h"

#include "Master.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "FontManager.h"
#include "ScreenConfig.h"

#include <string>
#include <random>	// std::mt19937
#include <iterator>	// std::size

namespace
{
	const std::string LOAD_UI_DIR = "Resource/UI/Scene_Load/";

	// -- お料理メモの一覧（データテーブル）。ロードのたびにここからランダムで1つ選ぶ -- //
	struct LoadTip
	{
		const char* title;		// 見出し
		const char* content;	// 本文
		const char* image;		// 添付画像（Resource/UI/Scene_Load/ 内。無ければ nullptr）
	};

	const LoadTip TIP_TABLE[] =
	{
		{ "【 お料理メモ：変形 】",			"「Eキー」を押すと変形できるよ！ 状況に合わせて豆腐の姿を変えよう。",		"変形！.png" },
		{ "【 お料理メモ：回転移動 】",		"「Aキー / Dキー」で左右にコロコロ転がって移動しよう！",					nullptr },
		{ "【 ギミック：スイッチブロック 】",	"スイッチを踏むと、同じ色のブロックが現れたり消えたりするよ。",				nullptr },
		{ "【 ギミック：トゲトゲ 】",			"赤いトゲにぶつかると豆腐が崩れちゃう！ 上手に飛び越えよう。",				"スパイク.png" },
		{ "【 ギミック：バターブロック 】",	"黄色いバターの上はツルツル滑るよ！ 滑る勢いを使って進もう。",				nullptr },
		{ "【 ギミック：崩れる足場 】",		"ヒビ割れたブロックは乗ると崩れるよ！ 立ち止まらず素早く渡ろう。",			nullptr },
		{ "【 攻略のコツ：形の使い分け 】",	"高い段差は「たて長」、低い天井のすき間は「よこ長」が通りやすいよ。",		"形状有利_1.png" },
		{ "【 やりこみ：調味料あつめ 】",		"ステージの調味料を3つ集めると、レシピ帳に料理が完成するよ！",				"やりこみ.png" },
		{ "【 攻略のヒント 】",				"「Eキー」で変形！ 困ったときはリスタートボタンを長押ししよう。",			"リスタート.png" },
	};

	constexpr int TIP_COUNT = static_cast<int>(std::size(TIP_TABLE));

	// ランダムに1つ選ぶ（前回と同じTipsが続けて出ないようにする）
	const LoadTip& PickRandomTip()
	{
		static std::mt19937 engine(std::random_device{}());	// 乱数の生成器（最初の1回だけ作る）
		static int lastIndex = -1;							// 前回選んだ番号（ゲーム起動中ずっと覚えておく）

		int index = 0;
		if (TIP_COUNT > 1)
		{
			std::uniform_int_distribution<int> dist(0, TIP_COUNT - 1);
			do
			{
				index = dist(engine);
			} while (index == lastIndex);	// 同じ番号なら引き直す
		}

		lastIndex = index;
		return TIP_TABLE[index];
	}

	constexpr float SCREEN_CX = ScreenConfig::SCREEN_WIDTH / 2.0f;
}

void LoadUI::Init()
{
	SetLayer(LAYER_BACKGROUND);

	const int stage = Master::mpSceneManager->GetStageNumber();
	const LoadTip& tip = PickRandomTip();

	CreateBackground();
	CreateLoadPanel(stage);
	CreateTipImage(tip.image);
	CreateTipPlate(tip.title, tip.content);

	SetEnabled(true);
}

// 子UIを作って、画面左上基準の「中心座標」と「大きさ」を設定する
template<class T>
T* LoadUI::CreateChild(int _layer, float _centerX, float _centerY, float _width, float _height)
{
	T* ui = GetUIManager()->CreateUI<T>();
	ui->SetLayer(_layer);

	auto& transform = ui->GetTransform();
	transform.SetAnchor(VGet(0.0f, 0.0f, 0.0f));	// 座標 = 画面のピクセル座標
	transform.SetPosition(VGet(_centerX, _centerY, 0.0f));
	transform.SetSize(VGet(_width, _height, 0.0f));

	AddChild(ui);
	ui->SetParent(this);
	ui->SetEnabled(true);
	return ui;
}

// 画面中央揃えのテキストを作る
UI* LoadUI::CreateText(float _centerY, FontId _fontId, unsigned int _color, const char* _text)
{
	UI* ui = CreateChild<UI>(LAYER_CONTENT, SCREEN_CX, _centerY);

	auto& text = ui->GetText();
	text.SetFontId(_fontId);
	text.SetColor(_color);
	text.SetEdgeColor(_color);	// 縁付きフォントでも縁が目立たないよう同色に
	text.SetText(_text);
	return ui;
}

void LoadUI::CreateBackground()
{
	const float sw = ScreenConfig::SCREEN_WIDTH;
	const float sh = ScreenConfig::SCREEN_HEIGHT;

	// 目にやさしい、ミルクティーのような薄い茶色
	auto* background = CreateChild<RoundRectUI>(LAYER_BACKGROUND, SCREEN_CX, sh / 2.0f, sw, sh);
	background->SetFillColor(GetColor(206, 178, 150));

	// 上下の飾りライン（StageBlockのレンガ色を少し落ち着かせた色）
	const unsigned int lineColor = GetColor(176, 120, 86);
	const float lineW = sw - 120.0f;

	auto* topLine = CreateChild<RoundRectUI>(LAYER_DECORATION, SCREEN_CX, 42.0f, lineW, 4.0f);
	topLine->SetFillColor(lineColor);

	auto* bottomLine = CreateChild<RoundRectUI>(LAYER_DECORATION, SCREEN_CX, sh - 42.0f, lineW, 4.0f);
	bottomLine->SetFillColor(lineColor);
}

void LoadUI::CreateLoadPanel(int _stageNumber)
{
	const float panelCY = PANEL_TOP + PANEL_H / 2.0f;

	// 影（背景が明るいので、半透明で柔らかく）
	auto* shadow = CreateChild<RoundRectUI>(LAYER_SHADOW, SCREEN_CX + 8.0f, panelCY + 8.0f, PANEL_W, PANEL_H);
	shadow->SetFillColor(GetColor(90, 60, 40));
	shadow->SetRadius(20);
	shadow->SetAlpha(70);

	// 白パネル
	auto* panel = CreateChild<NineSliceUI>(LAYER_BASE, SCREEN_CX, panelCY, PANEL_W, PANEL_H);
	panel->SetGraph(LOAD_UI_DIR + "Panel_white.png");
	panel->SetBorder(16, 16, 16, 16);

	// 「ステージ X」
	const std::string stageText = "ステージ " + std::to_string(_stageNumber);
	CreateText(PANEL_TOP + 48.0f, FontId::HUD, GetColor(185, 65, 45), stageText.c_str());

	// 湯気
	mpSteam = CreateChild<SteamUI>(LAYER_CONTENT, SCREEN_CX, PANEL_TOP + 135.0f);

	// 進行状況（文字と色は SetLoadState で更新）
	mpStatusText = CreateText(PANEL_TOP + 182.0f, FontId::HUD, GetColor(70, 50, 40), "");

	// 「画面をクリックしてスタート」（準備完了まで非表示）
	mpClickText = CreateChild<BlinkTextUI>(LAYER_CONTENT, SCREEN_CX, PANEL_TOP + 232.0f);
	auto& clickText = mpClickText->GetText();
	clickText.SetFontId(FontId::HUD);
	clickText.SetColor(GetColor(190, 55, 35));
	clickText.SetEdgeColor(GetColor(190, 55, 35));
	clickText.SetText(">>  画面をクリックしてスタート  <<");
	mpClickText->SetEnabled(false);

	// 出汁ゲージ
	mpProgressBar = CreateChild<ProgressBarUI>(LAYER_CONTENT, SCREEN_CX, PANEL_TOP + 278.0f, 720.0f, 22.0f);
}

void LoadUI::CreateTipImage(const char* _imageFile)
{
	if (_imageFile == nullptr)
		return;

	const float areaCY = (IMAGE_AREA_TOP + IMAGE_AREA_BOTTOM) / 2.0f;
	const float imageMaxH = (IMAGE_AREA_BOTTOM - IMAGE_AREA_TOP) - BOARD_PADDING * 2.0f;

	// 画像（持ち手の分だけ左に寄せて、まな板の「板」部分の中央に置く）
	const float imageCX = SCREEN_CX - BOARD_HANDLE_W / 2.0f;
	mpTipImage = CreateChild<RoundedImageUI>(LAYER_CONTENT, imageCX, areaCY);
	if (!mpTipImage->Setup(LOAD_UI_DIR + _imageFile, IMAGE_TARGET_AREA, IMAGE_MAX_W, imageMaxH, IMAGE_MAX_SCALE, IMAGE_CORNER_RADIUS))
	{
		mpTipImage->SetEnabled(false);
		mpTipImage = nullptr;
		return;
	}

	// まな板の大きさ = 画像 + 周りの余白 + 持ち手
	const VECTOR imageSize = mpTipImage->GetTransform().GetSize();
	const float boardW = imageSize.x + BOARD_PADDING * 2.0f + BOARD_HANDLE_W;
	const float boardH = imageSize.y + BOARD_PADDING * 2.0f;

	auto* shadow = CreateChild<RoundRectUI>(LAYER_SHADOW, SCREEN_CX + 6.0f, areaCY + 6.0f, boardW, boardH);
	shadow->SetFillColor(GetColor(90, 60, 40));
	shadow->SetRadius(16);
	shadow->SetAlpha(60);

	auto* board = CreateChild<NineSliceUI>(LAYER_BASE, SCREEN_CX, areaCY, boardW, boardH);
	board->SetGraph(LOAD_UI_DIR + "GraphPanel.png");
	board->SetBorder(14, 14, 48, 14);	// 右端は持ち手の穴ごと残す
}

void LoadUI::CreateTipPlate(const char* _title, const char* _content)
{
	const float plateCY = PLATE_TOP + PLATE_H / 2.0f;

	auto* shadow = CreateChild<RoundRectUI>(LAYER_SHADOW, SCREEN_CX + 6.0f, plateCY + 6.0f, PLATE_W, PLATE_H);
	shadow->SetFillColor(GetColor(90, 60, 40));
	shadow->SetRadius(18);
	shadow->SetAlpha(70);

	// 濃いこげ茶の板（文字とのコントラストを確保）
	auto* plate = CreateChild<RoundRectUI>(LAYER_BASE, SCREEN_CX, plateCY, PLATE_W, PLATE_H);
	plate->SetFillColor(GetColor(60, 42, 30));
	plate->SetOutlineColor(GetColor(215, 190, 150));
	plate->SetRadius(18);

	CreateText(PLATE_TOP + 42.0f, FontId::HUD, GetColor(255, 210, 85), _title);			// 見出し（山吹色）
	CreateText(PLATE_TOP + 105.0f, FontId::SubTitle, GetColor(255, 255, 255), _content);	// 本文（白）
}

void LoadUI::SetLoadState(float _elapsed, float _progress, bool _ready)
{
	mpSteam->SetTime(_elapsed);
	mpProgressBar->SetProgress(_progress);

	mpClickText->SetTime(_elapsed);
	mpClickText->SetEnabled(_ready);

	auto& status = mpStatusText->GetText();
	if (_ready)
	{
		status.SetText("したごしらえ完了！");
		status.SetColor(GetColor(45, 135, 55));
		status.SetEdgeColor(GetColor(45, 135, 55));
	}
	else
	{
		const int percent = static_cast<int>(_progress * 100.0f);
		status.SetText("したごしらえ中... " + std::to_string(percent) + "%");
		status.SetColor(GetColor(70, 50, 40));
		status.SetEdgeColor(GetColor(70, 50, 40));
	}
}

void LoadUI::Release()
{
	if (mpTipImage != nullptr)
		mpTipImage->Release();
}
