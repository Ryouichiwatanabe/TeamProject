#pragma once

#include <string>
#include <DxLib.h>

class UI;

enum class UITextAlign
{
	LEFT,
	CENTER,
	RIGHT
};

enum class UITextVerticalAlign
{
	TOP,
	CENTER,
	BOTTOM
};

class UIText
{
public:
	UIText() = default;
	~UIText() = default;

	void Initialize(UI* _pUI);

	void SetText(std::string _text) { mText = _text; }
	std::string GetText() { return mText; }

	void SetFontSize(int _size) { mnFontSize = _size; }
	void SetColor(unsigned int _color) { mColor = _color; }

	void SetAlign(UITextAlign _align) { mAlign = _align; }
	UITextAlign GetAlign() { return mAlign; }

	void Draw();

private:
	VECTOR CalculateTextDrawPosition();	// AlignによるX / Y を補正した座標を返す

private:
	std::string mText;

	int mnFontSize = 24;
	unsigned int mColor = GetColor(255, 255, 255);

	UITextAlign mAlign = UITextAlign::CENTER;

	UI* mpUI;
};