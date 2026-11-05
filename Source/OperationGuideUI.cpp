#include "OperationGuideUI.h"
#include "UIManager.h"

void OperationGuideUI::Init()
{
    // 自分自身が全画面の説明画像を持つ
    // LoadUIGraph("Resource/UI/Pause/OperationGuide.png");
    // SetLayer(3);    // Overlayやタイトル文字より手前に描く

    // -- 「A」ボタンの解説 -- //
    mpGuideA = GetUIManager()->CreateUI<UI>();
    mpGuideA->LoadUIGraph("Resource/UI/Pause/OperationGuide/A-Key.png");
    mpGuideA->GetTransform().SetScale(VGet(1.8f, 1.8f, 0.0f));
    mpGuideA->GetTransform().SetPosition(VGet( - 125.0f, -100.0f, 0.0f));
    mpGuideA->GetText().SetAlign(UITextAlign::RIGHT);
    mpGuideA->GetText().SetOffset(VGet(300.0f, 0.0f, 0.0f));
    mpGuideA->GetText().SetFontId(FontId::Button);
    mpGuideA->GetText().SetText("左回転");
    mpGuideA->SetEnabled(true);
    AddChild(mpGuideA);
    mpGuideA->SetParent(this);

    // -- 「D」ボタンの解説 -- //
    mpGuideD = GetUIManager()->CreateUI<UI>();
    mpGuideD->LoadUIGraph("Resource/UI/Pause/OperationGuide/D-Key.png");
    mpGuideD->GetTransform().SetScale(VGet(1.8f, 1.8f, 0.0f));
    mpGuideD->GetTransform().SetPosition(VGet(-125.0f, 0.0f, 0.0f));
    mpGuideD->GetText().SetAlign(UITextAlign::RIGHT);
    mpGuideD->GetText().SetOffset(VGet(300.0f, 0.0f, 0.0f));
    mpGuideD->GetText().SetFontId(FontId::Button);
    mpGuideD->GetText().SetText("右回転");
    mpGuideD->SetEnabled(true);
    AddChild(mpGuideD);
    mpGuideD->SetParent(this);

    // -- 「P」ボタンの解説 -- //
    mpGuideP = GetUIManager()->CreateUI<UI>();
    mpGuideP->LoadUIGraph("Resource/UI/Pause/OperationGuide/P-Key.png");
    mpGuideP->GetTransform().SetScale(VGet(1.8f, 1.8f, 0.0f));
    mpGuideP->GetTransform().SetPosition(VGet(-125.0f, 100.0f, 0.0f));
    mpGuideP->GetText().SetAlign(UITextAlign::RIGHT);
    mpGuideP->GetText().SetOffset(VGet(375.0f, 0.0f, 0.0f));
    mpGuideP->GetText().SetFontId(FontId::Button);
    mpGuideP->GetText().SetText("一時停止");
    mpGuideP->SetEnabled(true);
    AddChild(mpGuideP);
    mpGuideP->SetParent(this);

    // -- 「E」ボタンの解説 -- //
    mpGuideE = GetUIManager()->CreateUI<UI>();
    mpGuideE->LoadUIGraph("Resource/UI/Pause/OperationGuide/E-Key.png");
    mpGuideE->GetTransform().SetScale(VGet(1.8f, 1.8f, 0.0f));
    mpGuideE->GetTransform().SetPosition(VGet(-125.0f, 200.0f, 0.0f));
    mpGuideE->GetText().SetAlign(UITextAlign::RIGHT);
    mpGuideE->GetText().SetOffset(VGet(300.0f, 0.0f, 0.0f));
    mpGuideE->GetText().SetFontId(FontId::Button);
    mpGuideE->GetText().SetText("変形！");
    mpGuideE->SetEnabled(true);
    AddChild(mpGuideE);
    mpGuideE->SetParent(this);

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