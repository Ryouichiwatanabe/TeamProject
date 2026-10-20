#include "UIGraphic.h"

#include "UI.h"

#include "Master.h"
#include "ResourceManager.h"

#include "ScreenConfig.h"

#include <DxLib.h>	// 画像のサイズ取得用

void UIGraphic::Initialize(UI* _pUI, std::string _graphicFileName)
{
	// -- 所有権先のポインタを取得 -- //
	mpUI = _pUI;

	// -- 画像ロード -- //
	mnGraphHandle =
		Master::mpResource->LoadGraphics(_graphicFileName);

	// -- 画像のサイズ取得 -- //
	int size_x;
	int size_y;
	GetGraphSize(mnGraphHandle, &size_x, &size_y);

	mpUI->GetTransform().SetSize(
		VGet(
			static_cast<int>(size_x),
			static_cast<int>(size_y),
			0.0f
		)
	);
}

void UIGraphic::Draw()
{
	if (mnGraphHandle == -1)
		return;
	
	auto transform = mpUI->GetTransform();

	const auto centerPosition = transform.GetCenterPosition();
	const auto rotation = transform.GetRotation();
	const auto scale = transform.GetScale();
	const auto size = transform.GetSize();

	DrawRotaGraph3(
		static_cast<int>(centerPosition.x),
		static_cast<int>(centerPosition.y),
		static_cast<int>(size.x / 2.0f),
		static_cast<int>(size.y / 2.0f),
		scale.x,
		scale.y,
		rotation.z,
		mnGraphHandle,
		TRUE,
		FALSE
	);
}