#include "PlayerShapeUI.h"

// -- 所有するUI -- //
#include "PlayerPieceUI.h"

// -- UIの生成用 -- //
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIManager.h"

#include "ObjectManager.h"	// Playerのアドレス取得用
#include "Player.h"

namespace
{
	constexpr float BUTTON_OFFSET_X = 180.0f;	// OK / Back ボタンの左右位置
	constexpr float BUTTON_OFFSET_Y = 300.0f;	// OK / Back ボタンの縦位置
	constexpr float MESSAGE_OFFSET_Y = -280.0f;	// メッセージの縦位置

	constexpr int LAYER_FRAME = 0;
	constexpr int LAYER_PIECE = 1;
	constexpr int LAYER_BUTTON = 2;
}

void PlayerShapeUI::Init()
{
	LoadUIGraph("Resource/UI/外側.png");
	SetLayer(LAYER_FRAME);
	SetEnabled(false);	// 初期状態は非表示

	// -- piece生成 & 初期処理 -- //
	for (int row = 0; row < GameConfig::PLAYER_PIECE_SIZE; ++row)
	{
		for (int column = 0; column < GameConfig::PLAYER_PIECE_SIZE; ++column)
		{
			CreatePieceUI(column, row);
		}
	}

	// -- ボタン・メッセージ生成 -- //
	// Todo : 画像が決まり次第差し替え
	mpConfirmUI = CreateButtonUI("OK", -BUTTON_OFFSET_X, BUTTON_OFFSET_Y);
	mpBackUI = CreateButtonUI("Back", BUTTON_OFFSET_X, BUTTON_OFFSET_Y);
	mpMessageUI = CreateMessageUI(0.0f, MESSAGE_OFFSET_Y);

	// -- Playerのアドレス取得 -- //
	mpPlayer =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetObjectManager()
		->FindObject<Player>();
}

void PlayerShapeUI::CreatePieceUI(int _column, int _row)
{
	auto pieceUI = GetUIManager()->CreateUI<PlayerPieceUI>();
	if (pieceUI == nullptr)
		return;

	pieceUI->LoadUIGraph("Resource/UI/内側.png");
	pieceUI->SetLayer(LAYER_PIECE);
	mPieceUI[_column][_row] = pieceUI;

	// -- 親子の設定 -- //
	// 子は常に有効にしておき、表示/非表示は親(this)の有効状態だけで切り替える
	this->AddChild(pieceUI);
	pieceUI->SetParent(this);
	pieceUI->SetEnabled(true);

	// -- 初期座標の設定 (中央マス(1,1)を原点とする) -- //
	auto& pieceTransform = pieceUI->GetTransform();
	const VECTOR size = pieceTransform.GetSize();
	pieceTransform.SetPosition(VGet(
		(_column - 1) * size.x,
		(_row - 1) * size.y,
		0.0f
	));
}

UI* PlayerShapeUI::CreateButtonUI(const std::string& _text, float _x, float _y)
{
	UI* button = GetUIManager()->CreateUI<UI>();

	button->LoadUIGraph("Resource/UI/内側.png");
	button->SetLayer(LAYER_BUTTON);
	button->GetTransform().SetScale(VGet(2.0f, 1.0f, 1.0f));
	button->GetTransform().SetPosition(VGet(_x, _y, 0.0f));
	button->GetText().SetText(_text);

	this->AddChild(button);
	button->SetParent(this);
	button->SetEnabled(true);

	return button;
}

UI* PlayerShapeUI::CreateMessageUI(float _x, float _y)
{
	// 画像なし・テキストのみのUI
	UI* message = GetUIManager()->CreateUI<UI>();

	message->SetLayer(LAYER_BUTTON);
	message->GetTransform().SetPosition(VGet(_x, _y, 0.0f));
	message->GetText().SetColor(GetColor(255, 80, 80));

	this->AddChild(message);
	message->SetParent(this);
	message->SetEnabled(true);

	return message;
}

void PlayerShapeUI::Open()
{
	if (mpPlayer == nullptr)
		return;

	// 基盤ブロックを決めて Player を停止 → その状態をUIへ反映
	mpPlayer->BeginShapeEdit();
	SyncFromPlayer();
	ShowMessage("");

	SetEnabled(true);
}

void PlayerShapeUI::Close()
{
	if (mpPlayer != nullptr)
		mpPlayer->EndShapeEdit();

	SetEnabled(false);
}

void PlayerShapeUI::Update()
{
	UI::Update();

	if (mpConfirmUI->GetButton().IsClicked())
	{
		ApplyShapeFromUI();
		return;
	}

	// Back ボタンで変更せずに閉じる (変形キーでの開閉は GameScene 側)
	if (mpBackUI->GetButton().IsClicked())
	{
		Close();
		return;
	}
}

void PlayerShapeUI::Draw()
{
	UI::Draw();
}

bool PlayerShapeUI::IsPieceSelected(int _column, int _row) const
{
	return mPieceUI[_column][_row]->IsSelected();
}

void PlayerShapeUI::SyncFromPlayer()
{
	const PlayerShapeMask shape = mpPlayer->GetScreenShape();

	int baseColumn = -1;
	int baseRow = -1;
	mpPlayer->GetScreenBaseCell(baseColumn, baseRow);

	for (int row = 0; row < GameConfig::PLAYER_PIECE_SIZE; ++row)
	{
		for (int column = 0; column < GameConfig::PLAYER_PIECE_SIZE; ++column)
		{
			auto pieceUI = mPieceUI[column][row];
			pieceUI->SetSelected(shape[column][row]);
			pieceUI->SetBase(column == baseColumn && row == baseRow);
		}
	}
}

void PlayerShapeUI::ApplyShapeFromUI()
{
	if (mpPlayer == nullptr)
		return;

	PlayerShapeMask shape{};
	for (int row = 0; row < GameConfig::PLAYER_PIECE_SIZE; ++row)
	{
		for (int column = 0; column < GameConfig::PLAYER_PIECE_SIZE; ++column)
		{
			shape[column][row] = IsPieceSelected(column, row);
		}
	}

	switch (mpPlayer->TryChangeShapeFromScreen(shape))
	{
	case ShapeChangeResult::Success:
		Close();
		break;

	case ShapeChangeResult::BaseRemoved:
		ShowMessage("基盤ブロック(赤枠)は外せません");
		break;

	case ShapeChangeResult::Disconnected:
		ShowMessage("ブロックはひとつにつなげてください");
		break;

	case ShapeChangeResult::Overlapping:
		ShowMessage("地形に重なるため変形できません");
		break;
	}
}

void PlayerShapeUI::ShowMessage(const std::string& _message)
{
	if (mpMessageUI != nullptr)
		mpMessageUI->GetText().SetText(_message);
}
