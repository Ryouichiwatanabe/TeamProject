#pragma once

#include "UI.h"
#include "GameConfig.h"
#include "ItemRecord.h"   // StageFlags
#include <array>

class ItemIconUI;

class ClearUI : public UI
{
public:
    void Init() override;
    void Update() override;

    // クリア時に呼ぶ：今回の取得状態をアイコンに反映して表示する
    void Show(const ItemRecord::StageFlags& _picked);

private:
    void CreateItemIcons();             // ItemアイコンのUIを生成する純粋関数
    void CreateStageSelectButton();     // 「ステージ選択シーンへ」のUIを生成する純粋関数
    void CreatePanel();

private:
    static constexpr float ICON_Y = 0.0f;
    static constexpr float ICON_SPACING = 200.0f;  // アイコン同士の間隔（画像の幅より大きく）
    static constexpr float BUTTON_Y = 300.0f;

    std::array<ItemIconUI*, GameConfig::MAX_ITEMS_PER_STAGE> mpIcons{}; // ItemアイコンのUI
    UI* mpStageSelectUI = nullptr;                                      // 「ステージ選択シーンへ」のボタン
    UI* mpPanel = nullptr;                                              // パネル
};