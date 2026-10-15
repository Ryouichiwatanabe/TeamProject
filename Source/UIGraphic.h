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

private:
	int mnGraphHandle = -1;

	UI* mpUI;
};