#include "ItemHUD.h"
#include "UIManager.h"
#include "ItemTable.h"

void ItemHUD::Init() {
	SetEnabled(true);
	SetLayer(LAYER_HUD);
	auto* uiManager = GetUIManager();

	for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; ++i) {
		const std::string path = ItemTable::GetFilePath(mStage, i);
		if (path.empty())
			continue;

		UI* icon = uiManager->CreateUI<UI>();
		icon->LoadUIGraph(path);
		icon->SetLayer(LAYER_HUD);		// ★子にも個別に設定（親からは引き継がれない）

		auto& transform = icon->GetTransform();
		transform.SetAnchor(VGet(0.0f, 0.0f, 0.0f));
		transform.SetPosition(VGet(START_X + i * SPACING, START_Y, 0.0f));
		transform.SetScale(VGet(ICON_SCALE, ICON_SCALE, 1.0f));

		AddChild(icon);
		icon->SetParent(this);
		icon->SetEnabled(false);		// 取るまで非表示

		mpIcons[i] = icon;
	}
}

void ItemHUD::SetPicked(int _slot) {
	if (_slot < 0 || _slot >= GameConfig::MAX_ITEMS_PER_STAGE || mpIcons[_slot] == nullptr)
		return;
	mpIcons[_slot]->SetEnabled(true);
}