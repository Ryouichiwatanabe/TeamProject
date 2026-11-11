#include "UIGraphic.h"

#include "UI.h"
#include "UIHoverScaler.h"

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
			static_cast<float>(size_x),
			static_cast<float>(size_y),
			0.0f
		)
	);
}

void UIGraphic::Draw()
{
	if (mnGraphHandle == -1)
		return;
	
	auto& transform = mpUI->GetTransform();
	const float hover = mpUI->GetHoverScaler().GetScale();
	const VECTOR floating = mpUI->GetFloatingMotion().GetOffset();

	auto centerPosition = transform.GetCenterPosition();
	centerPosition = VAdd(centerPosition, floating);

	const auto rotation = transform.GetRotation();
	const auto scale = transform.GetScale();
	const auto size = transform.GetSize();

	// 明るさを変える場合だけ、元の値を保存して描画後に戻す
	const bool changeBright = (mnBright != 255);
	int prevR = 255, prevG = 255, prevB = 255;
	if (changeBright)
	{
		GetDrawBright(&prevR, &prevG, &prevB);
		SetDrawBright(mnBright, mnBright, mnBright);
	}

	DrawRotaGraph3(
		static_cast<int>(centerPosition.x),
		static_cast<int>(centerPosition.y),
		static_cast<int>(size.x / 2.0f),
		static_cast<int>(size.y / 2.0f),
		scale.x * hover,
		scale.y * hover,
		rotation.z,
		mnGraphHandle,
		TRUE,
		FALSE
	);

	if (changeBright)
	{
		SetDrawBright(prevR, prevG, prevB);
	}
}