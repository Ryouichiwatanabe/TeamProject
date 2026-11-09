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

	// -- floatingを適用(有効でない場合はデフォルト) -- //
	const VECTOR floating = mpUI->GetFloatingMotion().GetOffset();
	position = VAdd(position, floating);

	DrawStringToHandle(
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		mText.c_str(),
		mColor,
		fontHandle,
		mEdgeColor
	);

	if (mbUnderline)
		DrawUnderline(position, fontHandle);
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

void UIText::DrawUnderline(const VECTOR& _textPosition, int _fontHandle) const
{
	const int width = GetDrawStringWidthToHandle(
		mText.c_str(),
		static_cast<int>(mText.size()),
		_fontHandle
	);
	const int height = GetFontSizeToHandle(_fontHandle);

	const int left = static_cast<int>(_textPosition.x) - UNDERLINE_PADDING;
	const int right = static_cast<int>(_textPosition.x) + width + UNDERLINE_PADDING;
	const int top = static_cast<int>(_textPosition.y) + height + UNDERLINE_GAP;

	DrawBox(left, top, right, top + UNDERLINE_THICKNESS, mColor, TRUE);
}