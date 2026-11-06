#include "ItemHUD.h"
#include "UIManager.h"
#include "ItemTable.h"
#include "ItemIconUI.h"

void ItemHUD::Init() {
	SetEnabled(true);
	SetLayer(LAYER_HUD);
	auto* uiManager = GetUIManager();

	for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; ++i) {
		const std::string path = ItemTable::GetFilePath(mStage, i);
		if (path.empty())
			continue;

		ItemIconUI* icon = uiManager->CreateUI<ItemIconUI>();
		icon->LoadUIGraph(path);
		icon->SetLayer(LAYER_HUD);		// ★子にも個別に設定（親からは引き継がれない）

		auto& transform = icon->GetTransform();
		transform.SetAnchor(VGet(1.0f, 0.0f, 0.0f));
		transform.SetPosition(VGet(START_X + i * SPACING, START_Y, 0.0f));
		transform.SetScale(VGet(ICON_SCALE, ICON_SCALE, 1.0f));

		AddChild(icon);
		icon->SetParent(this);
		icon->SetEnabled(true);

		mpIcons[i] = icon;
	}
}

void ItemHUD::SetPicked(int _slot) {
	if (_slot < 0 || _slot >= GameConfig::MAX_ITEMS_PER_STAGE || mpIcons[_slot] == nullptr)
		return;
	mpIcons[_slot]->SetPicked(true);	// 「通常描画」に切り替え
}