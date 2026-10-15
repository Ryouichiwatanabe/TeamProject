#include "UIText.h"

#include "UI.h"

void UIText::Initialize(UI* _pUI)
{
	mpUI = _pUI;
}

void UIText::Draw()
{
	VECTOR position = CalculateTextDrawPosition();

	DrawString(
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		mText.c_str(),
		mColor
	);
}

VECTOR UIText::CalculateTextDrawPosition()
{
	auto transform =
		mpUI->GetTransform();
	/*VECTOR basePosition = VSub(
		transform.GetCenterPosition(),
		transform.GetSize()
	);*/

	VECTOR basePosition = transform.GetCenterPosition();

	int textWidth = GetDrawStringWidth(
		mText.c_str(),
		static_cast<int>(mText.size()),
		mnFontSize
	);

	int drawX = static_cast<int>(basePosition.x);

	switch (mAlign)
	{
	case UITextAlign::LEFT:
		break;

	case UITextAlign::CENTER:
		drawX -= textWidth / 2;
		break;

	case UITextAlign::RIGHT:
		drawX -= textWidth;
		break;
	}

	int drawY = static_cast<int>(basePosition.y - mnFontSize / 2.0f);

	return VGet(
		static_cast<int>(drawX),
		static_cast<int>(drawY),
		0.0f
	);
}