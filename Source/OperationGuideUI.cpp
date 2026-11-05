#include "OperationGuideUI.h"
#include "UIManager.h"

void OperationGuideUI::Init()
{
    // 自分自身が全画面の説明画像を持つ
    // LoadUIGraph("Resource/UI/Pause/OperationGuide.png");
    // SetLayer(3);    // Overlayやタイトル文字より手前に描く



    // -- 「戻る」ボタン -- //
    mpBackButton = GetUIManager()->CreateUI<UI>();
    mpBackButton->LoadUIGraph("Resource/UI/Pause/Pause_button.png");
    mpBackButton->GetText().SetFontId(FontId::SubTitle);
    mpBackButton->GetText().SetText("戻る");
    mpBackButton->SetLayer(4);
    mpBackButton->GetTransform().SetPosition(VGet(0.0f, 400.0f, 0.0f));
    mpBackButton->SetEnabled(true);     // 表示は親（自分）に従う
    AddChild(mpBackButton);
    mpBackButton->SetParent(this);
}

void OperationGuideUI::Update()
{
    UI::Update();
    mbBackRequested = mpBackButton->GetButton().IsClicked();
}