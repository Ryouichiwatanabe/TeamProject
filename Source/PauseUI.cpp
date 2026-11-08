#include "PauseUI.h"

#include "Master.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "Scene.h"
#include "UIManager.h"

#include "OperationGuideUI.h"

void PauseUI::Init()
{
    SetEnabled(false);
    GetText().SetFontId(FontId::Title);
    GetText().SetText("Pause");
    GetText().SetOffset(VGet(0.0f, -225.0f, 0.0f));
    SetLayer(1);

    auto* uiManager = GetUIManager();

    // オーバーレイ
    mpPauseOverlay = uiManager->CreateUI<UI>();
    mpPauseOverlay->LoadUIGraph("Resource/UI/Overlay/Overlay_black.png");
    mpPauseOverlay->SetEnabled(true);
    AddChild(mpPauseOverlay);
    mpPauseOverlay->SetParent(this);

    // メニューのまとめ役
    mpMenuRoot = uiManager->CreateUI<UI>();
    mpMenuRoot->SetEnabled(true);
    AddChild(mpMenuRoot);
    mpMenuRoot->SetParent(this);

    mpResumeUI = CreateMenuButton("再開", -120.0f);
    mpStageSelectUI = CreateMenuButton("ステージ選択に戻る", 20.0f);
    mpTitleUI = CreateMenuButton("タイトルへ戻る", 160.0f);
    mpOperationButton = CreateMenuButton("操作説明", 300.0f);

    // 操作説明
    mpGuideUI = uiManager->CreateUI<OperationGuideUI>();
    AddChild(mpGuideUI);
    mpGuideUI->SetParent(this);

    ChangeState(State::Menu);
}

void PauseUI::AddMenuContent(UI* _ui)
{
    if (_ui == nullptr)
        return;

    mpMenuRoot->AddChild(_ui);
    _ui->SetParent(mpMenuRoot);	// メニュー表示中だけ見える（操作説明中は消える）
}

UI* PauseUI::CreateMenuButton(const std::string& _text, float _y)
{
    UI* button = GetUIManager()->CreateUI<UI>();
    button->LoadUIGraph("Resource/UI/Pause/Pause_button.png");
    button->GetText().SetFontId(FontId::SubTitle);
    button->GetText().SetText(_text);
    button->SetLayer(2);
    button->GetTransform().SetPosition(VGet(0.0f, _y, 0.0f));
    button->SetEnabled(true);   // 表示はMenuRootに従う
    mpMenuRoot->AddChild(button);
    button->SetParent(mpMenuRoot);
    return button;
}

void PauseUI::ChangeState(State _next)
{
    mState = _next;
    mpMenuRoot->SetEnabled(mState == State::Menu);
    mpGuideUI->SetEnabled(mState == State::Guide);
}

void PauseUI::Update()
{
    UI::Update();

    // 今の状態のボタンだけを判定する（①②の対策）
    switch (mState)
    {
    case State::Menu:  UpdateMenu();  break;
    case State::Guide: UpdateGuide(); break;
    }
}

void PauseUI::UpdateMenu()
{
    if (mpResumeUI->GetButton().IsClicked())
    {
        SetEnabled(false);
        return;
    }
    if (mpStageSelectUI->GetButton().IsClicked())
    {
        SoundManager::GetInstance()->StopBGM();
        SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title);
        Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE_EX);
        return;
    }
    if (mpTitleUI->GetButton().IsClicked())
    {
        SoundManager::GetInstance()->StopBGM();
        Master::mpSceneManager->RequestScene(SCENE_TYPE::TITLE_SCENE);
        return;
    }
    if (mpOperationButton->GetButton().IsClicked())
    {
        ChangeState(State::Guide);
        return;     // 同じフレームで戻るボタンを判定しない
    }
}

void PauseUI::UpdateGuide()
{
    if (mpGuideUI->IsBackRequested())
        ChangeState(State::Menu);
}

void PauseUI::OnPauseInput()
{
    // 操作説明中なら → メニューへ戻るだけ
    if (IsEnabled() && mState == State::Guide)
    {
        ChangeState(State::Menu);
        return;
    }
    // 開くときは必ずメニューから（③の対策）
    ChangeState(State::Menu);
    ToggleEnabled();
}