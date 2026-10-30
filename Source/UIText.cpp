#include "UIText.h"

#include "UI.h"

#include "Master.h"
#include "FontManager.h"

void UIText::Initialize(UI* _pUI)
{
	mpUI = _pUI;
}

void UIText::Draw()
{
	if (mpUI == nullptr || mText.empty())
		return;

	// -- フォントハンドルを取得(未登録でもDefaultが返るので-1チェック不要) -- //
	int fontHandle = Master::mpFontManager->GetHandle(mFontId);

	VECTOR position = CalculateTextDrawPosition(fontHandle);

	DrawStringToHandle(
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		mText.c_str(),
		mColor,
		fontHandle,
		mEdgeColor
	);
}

VECTOR UIText::CalculateTextDrawPosition(int _fontHandle) const
{
	auto& transform = mpUI->GetTransform();
	VECTOR basePosition = transform.GetCenterPosition();

	// -- 描画サイズを「実際に使うフォント」から取得 -- //
	int textWidth = GetDrawStringWidthToHandle(
		mText.c_str(),
		static_cast<int>(mText.size()),
		_fontHandle
	);
	int textHeight = GetFontSizeToHandle(_fontHandle);

	// -- 横の配置 -- //
	float drawX = basePosition.x;
	switch (mAlign)
	{
	case UITextAlign::LEFT:
		break;

	case UITextAlign::CENTER:
		drawX -= textWidth / 2.0f;
		break;

	case UITextAlign::RIGHT:
		drawX -= textWidth;
		break;
	}

	// -- 縦の配置 -- //
	float drawY = basePosition.y;
	switch (mVerticalAlign)
	{
	case UITextVerticalAlign::TOP:
		break;

	case UITextVerticalAlign::CENTER:
		drawY -= textHeight / 2.0f;
		break;

	case UITextVerticalAlign::BOTTOM:
		drawY -= textHeight;
		break;
	}

	return VAdd(VGet(drawX, drawY, 0.0f), mvOffset);
}
