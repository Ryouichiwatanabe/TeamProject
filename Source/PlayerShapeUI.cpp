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

// -- Module -- //
#include "Transform.h"

#include "GameConfig.h"

#include "PlayerPiece.h"

void PlayerShapeUI::Init()
{
	// -- Playerのアドレス取得 -- //
	mpPlayer =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetObjectManager()
		->FindObject<Player>();
	mpPlayer->SetShapeUI(this);

	// -- piece生成 & 初期処理 -- //
	for (int row = 0;
		row < GameConfig::PLAYER_PIECE_SIZE;
		++row)
	{
		for (int column = 0;
			column < GameConfig::PLAYER_PIECE_SIZE;
			++column)
		{
			CreatePieceUI(column, row);
		}
	}

	// -- 「決定ボタン」の生成 / 初期処理 -- //
		// Todo : 関数化
	mpConfirmUI =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetUIManager()
		->CreateUI<UI>();
	mpConfirmUI->LoadUIGraph("Resource/Obj/test_field.png");
	mpConfirmUI->SetLayer(2);
	mpConfirmUI
		->GetTransform()
		.SetScale(
			VGet(2.0f,
				1.0f,
				1.0f
			)
		);
	mpConfirmUI
		->GetTransform().SetPosition(
			VGet(
				-180.0f,
				300.0f,
				0.0f
			)
		);
	mpConfirmUI
		->GetText()
		.SetText(
			"OK"
		);

	this->AddChild(mpConfirmUI);
	mpConfirmUI->SetParent(this);


	// -- 「戻るボタン」の生成 / 初期処理 -- //
		// Todo : 関数化
	mpBackUI =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetUIManager()
		->CreateUI<UI>();
	mpBackUI->LoadUIGraph("Resource/Obj/test_field.png");
	mpBackUI->SetLayer(2);
	mpBackUI
		->GetTransform()
		.SetScale(
			VGet(2.0f,
				1.0f,
				1.0f
			)
		);
	mpBackUI
		->GetTransform().SetPosition(
			VGet(
				180.0f,
				300.0f,
				0.0f
			)
		);
	mpBackUI
		->GetText()
		.SetText(
			"Back"
		);

	this->AddChild(mpBackUI);
	mpBackUI->SetParent(this);
}

void PlayerShapeUI::CreatePieceUI(int _column, int _row)
{
	// -- PieceUIの生成 -- //
	auto pieceUI =
		Master::mpSceneManager
		->GetCurrentScene()
		->GetUIManager()
		->CreateUI<PlayerPieceUI>();

	if (pieceUI == nullptr)
		return;

	pieceUI->LoadUIGraph("Resource/UI/内側.png");
	pieceUI->SetPlayerPiece(mpPlayer->GetPlayerPiece(_column, _row));
	pieceUI->SetLayer(1);
	mPieceUI[_row][_column] = pieceUI;

	// -- 親子の設定 -- //
	this->AddChild(pieceUI);
	pieceUI->SetParent(this);

	// -- 初期座標の設定 -- //
	auto& pieceTransform = pieceUI->GetTransform();
	VECTOR size = pieceTransform.GetSize();
	VECTOR localPosition = VGet(
		(_column - 1) * size.x,
		(_row - 1) * size.y,
		0.0f
	);

	pieceTransform.SetPosition(localPosition);
}

void PlayerShapeUI::Update()
{
	UI::Update();

	// -- 「OK」を押した場合 -- //
	if (mpConfirmUI->GetButton().IsClicked())
	{
		mpPlayer->ApplyUISelectionToPlayerPiece();	// UI(Piece)の選択状態をObjに適用
		this->SetEnabled(false);					// UIを閉じる
	}

	// -- 「Back」を押した場合 -- //
	if (mpBackUI->GetButton().IsClicked())
	{
		this->SetEnabled(false);					// UIを閉じる
	}

	if (IsEnabled())
	{
		for (int row = 0;
			row < GameConfig::PLAYER_PIECE_SIZE;
			++row)
		{
			for (int column = 0;
				column < GameConfig::PLAYER_PIECE_SIZE;
				++column)
			{
				mPieceUI[column][row]->SetEnabled(true);
				mpConfirmUI->SetEnabled(true);
				mpBackUI->SetEnabled(true);
			}
		}
	}

	else
	{
		for (int row = 0;
			row < GameConfig::PLAYER_PIECE_SIZE;
			++row)
		{
			for (int column = 0;
				column < GameConfig::PLAYER_PIECE_SIZE;
				++column)
			{
				mPieceUI[column][row]->SetEnabled(false);
				mpConfirmUI->SetEnabled(false);
				mpBackUI->SetEnabled(false);
			}
		}
	}
}

void PlayerShapeUI::Draw()
{
	UI::Draw();
}

void PlayerShapeUI::ApplyPieceActivityToUI()
{
	if (!mpPlayer)
		return;

	for (int row = 0;
		row < GameConfig::PLAYER_PIECE_SIZE;
		++row)
	{
		for (int column = 0;
			column < GameConfig::PLAYER_PIECE_SIZE;
			++column)
		{
			auto piece = mpPlayer->GetPlayerPiece(column, row);
			if (!piece)
				continue;

			auto pieceUI = GetPlayerPieceUI(column, row);
			if (!pieceUI)
				continue;

			pieceUI->SetSelected(piece->IsActive());
		}
	}
}


