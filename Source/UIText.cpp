#include "UIText.h"

#include "UI.h"

void UIText::Initialize(UI* _pUI)
{
	mpUI = _pUI;
}

void UIText::Draw()
{
	std::string drawText = mText;	// 描画する文字列の宣言

	// -- 全体の文字列から描画分の文字列を取得 -- //
	/*drawText = mpUI
		->GetTypewriterText()
		.GetVisibleText();*/

	VECTOR position = CalculateTextDrawPosition(drawText);

	DrawString(
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		drawText.c_str(),
		mColor
	);
}

VECTOR UIText::CalculateTextDrawPosition(std::string _text)
{
	auto transform =
		mpUI->GetTransform();

	VECTOR basePosition = transform.GetCenterPosition();

	int textWidth = GetDrawStringWidth(
		_text.c_str(),
		static_cast<int>(_text.size()),
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

	int drawY = 
		static_cast<int>(basePosition.y - mnFontSize / 2.0f);

	return VGet(
		static_cast<int>(drawX),
		static_cast<int>(drawY),
		0.0f
	);
}