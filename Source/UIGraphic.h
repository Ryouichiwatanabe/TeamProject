#pragma once
#include <string>

class UI;
class UIGraphic
{
public:
	UIGraphic() = default;
	~UIGraphic() = default;

	void Initialize(UI* _pUI, std::string _graphicFileName);

	void Draw();

	// 画像の明るさ (0〜255, 255 = 元の色のまま)
	void SetBright(int _bright) { mnBright = _bright; }
	int GetBright() const { return mnBright; }

private:
	int mnGraphHandle = -1;
	int mnBright = 255;

	UI* mpUI;
};