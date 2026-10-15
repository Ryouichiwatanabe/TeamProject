#pragma once
#include <string>

class UI;
class UIGraphic
{
public:
	UIGraphic() = default;
	~UIGraphic() = default;

	void Initialize(UI* _pUI, std::string _graphicFileName);

	// 表示する画像を変更
	void SetGraph(std::string _graphicFileName);

	void Draw();

private:
	int mnGraphHandle = -1;

	UI* mpUI;
};