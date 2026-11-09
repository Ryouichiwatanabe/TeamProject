#include "ClearUI.h"
#include "Master.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "ItemIconUI.h"
#include "ItemTable.h"
#include "FontManager.h"

void ClearUI::Init()
{
    SetEnabled(false);
    SetLayer(0);

    CreateItemIcons();
    CreateStageSelectButton();
    CreatePanel();
}

void ClearUI::CreateItemIcons()
{
    const int stage = Master::mpSceneManager->GetStageNumber();

    // 真ん中のアイコンが x = 0 に来るようにずらす（3個なら -1, 0, +1 倍）
    constexpr float CENTER_INDEX = (GameConfig::MAX_ITEMS_PER_STAGE - 1) / 2.0f;

    for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; i++)
    {
        const std::string path = ItemTable::GetFilePath(stage, i);
        if (path.empty())
            continue;

        ItemIconUI* icon = GetUIManager()->CreateUI<ItemIconUI>();
        icon->LoadUIGraph(path);
        icon->SetLayer(2);
        icon->GetTransform().SetPosition(
            VGet((i - CENTER_INDEX) * ICON_SPACING, ICON_Y, 0.0f));

        AddChild(icon);
        icon->SetParent(this);
        icon->SetEnabled(true);     // 表示は ClearUI に従う。未取得は暗く描かれる

        mpIcons[i] = icon;
    }
}

void ClearUI::CreateStageSelectButton()
{
    mpStageSelectUI = GetUIManager()->CreateUI<UI>();
    mpStageSelectUI->LoadUIGraph("Resource/UI/Clear/button.png");
    mpStageSelectUI->SetLayer(2);
    mpStageSelectUI->GetTransform().SetPosition(VGet(0.0f, BUTTON_Y, 0.0f));

    mpStageSelectUI->GetText().SetFontId(FontId::SubTitle);
    mpStageSelectUI->GetText().SetText("ステージ選択へ戻る");
    mpStageSelectUI->GetText().SetColor(GetColor(255, 255, 255));

    AddChild(mpStageSelectUI);
    mpStageSelectUI->SetParent(this);
    mpStageSelectUI->SetEnabled(true);
}

void ClearUI::CreatePanel() 
{
    mpPanel = GetUIManager()->CreateUI<UI>();
    mpPanel->LoadUIGraph("Resource/UI/Clear/test.png");
    mpPanel->SetLayer(1);

    mpPanel->GetText().SetFontId(FontId::SubTitle);
    mpPanel->GetText().SetText("Clear!!");
    mpPanel->GetText().SetOffset(VGet(0.0f, -300.0f, 0.0f));
    mpPanel->GetText().SetColor(GetColor(255, 255, 255));

    AddChild(mpPanel);
    mpPanel->SetParent(this);
    mpPanel->SetEnabled(true);
}

void ClearUI::Show(const ItemRecord::StageFlags& _picked)
{
    for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; i++)
    {
        if (mpIcons[i] != nullptr)
            mpIcons[i]->SetPicked(_picked[i]);
    }
    SetEnabled(true);
}

void ClearUI::Update()
{
    UI::Update();

    if (mpStageSelectUI->GetButton().IsClicked())
        Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE_EX);
}