#pragma once

#include <string>
#include <DxLib.h>

#include "FontManager.h" // FontId

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

/* memo :
* 各UIにコンポジットする「テキスト描画」の要素。
* フォントの見た目(名前・サイズ・太さ)はFontIdで指定し、FontManagerから取得する。
* 色・縁の色・配置はこのクラスで管理する。
*/
class UIText
{
public:
	UIText() = default;
	~UIText() = default;

	void Initialize(UI* _pUI);
	void Draw();

	// -- テキスト -- //
	void SetText(const std::string& _text) { mText = _text; }
	const std::string& GetText() const { return mText; }

	// -- フォント -- //
	void SetFontId(FontId _fontId) { mFontId = _fontId; }
	FontId GetFontId() const { return mFontId; }

	// -- 色 -- //
	void SetColor(unsigned int _color) { mColor = _color; }
	void SetEdgeColor(unsigned int _edgeColor) { mEdgeColor = _edgeColor; }	// 縁付きフォントのときのみ有効

	// -- 配置 -- //
	void SetAlign(UITextAlign _align) { mAlign = _align; }
	UITextAlign GetAlign() const { return mAlign; }

	void SetVerticalAlign(UITextVerticalAlign _align) { mVerticalAlign = _align; }
	UITextVerticalAlign GetVerticalAlign() const { return mVerticalAlign; }

	void SetOffset(const VECTOR& _offset) { mvOffset = _offset; }

private:
	VECTOR CalculateTextDrawPosition(int _fontHandle) const;	// Alignを考慮した描画座標を返す

private:
	std::string mText;	// 描画するテキストの全文

	FontId mFontId = FontId::Default;	// 使用するフォント

	unsigned int mColor = GetColor(255, 255, 255);	// 文字色
	unsigned int mEdgeColor = GetColor(0, 0, 0);	// 縁の色

	UITextAlign mAlign = UITextAlign::CENTER;					// 横の配置(デフォルトは中央揃え)
	UITextVerticalAlign mVerticalAlign = UITextVerticalAlign::CENTER;	// 縦の配置(デフォルトは中央揃え)

	VECTOR mvOffset = VGet(0.0f, 0.0f, 0.0f);	// 配置後の追加オフセット

	UI* mpUI = nullptr;	// 付与先のUI
};
