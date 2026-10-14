#include "PlayerPieceUI.h"

#include "PlayerPiece.h"

#include "Debug.h"

#include <DxLib.h>	// 選択済みPieceの減算ブレンド用
#include "InputManager.h"

void PlayerPieceUI::Init()
{
	LoadUIGraph("Resource/Obj/test_field.png");

	auto& transform = GetTransform();
	transform.SetAnchor(VGet(0.5f, 0.5f, 0.0f));
	transform.SetPosition(VGet(100.0f, 100.0f, 0.0f));

	auto& text = GetText();
	text.SetText("うんこ");
}

void PlayerPieceUI::Update()
{
	if (GetButton().IsClicked())
		ToggleSelected();
}

void PlayerPieceUI::Draw()
{
	if (IsSelected())
	{
		SetDrawBlendMode(DX_BLENDMODE_SUB, 100);
		UI::Draw();
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		return;
	}

	UI::Draw();
}

bool PlayerPieceUI::IsSelected()
{
	return mbSelected;
}

void PlayerPieceUI::SetSelected(bool _selected)
{
	mbSelected = _selected;

	if (mpPlayerPiece)
		mpPlayerPiece->SetEnabled(_selected);
}

void PlayerPieceUI::ToggleSelected()
{
	mbSelected = !mbSelected;
	SetSelected(mbSelected);
}