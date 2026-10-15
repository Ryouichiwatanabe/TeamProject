#include "UI.h"

void UI::Initialize(UIManager* _manager)
{
	// Todo : 初期処理何かあり次第追加
	mpUIManager = _manager;

	mText.Initialize(this);
	mButton.Initialize(this);

	Init();
}

void UI::LoadUIGraph(std::string filename)
{
	mGraphic.Initialize(this, filename);
}

void UI::Draw()
{
	// Todo : 要素が増え次第追加検討(主にstring)
	mGraphic.Draw();
	mText.Draw();
}

bool UI::IsEnabled()
{
	if (!mbIsEnabled)
		return false;

	if (mpParent != nullptr)
		return mpParent->IsEnabled();

	return true;
}