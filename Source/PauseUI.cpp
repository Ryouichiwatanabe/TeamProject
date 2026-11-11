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

    auto* uiManager = GetUIManager();

    // オーバーレイ（変更なし）
    mpPauseOverlay = uiManager->CreateUI<UI>();
    mpPauseOverlay->LoadUIGraph("Resource/UI/Overlay/Overlay_black.png");
    mpPauseOverlay->SetEnabled(true);
    AddChild(mpPauseOverlay);
    mpPauseOverlay->SetParent(this);

    // メニューのまとめ役（変更なし）
    mpMenuRoot = uiManager->CreateUI<UI>();
    mpMenuRoot->SetEnabled(true);
    AddChild(mpMenuRoot);
    mpMenuRoot->SetParent(this);

    // 冷蔵庫（MenuRootの子なので操作説明中は消える)
    mpFridgeUI = uiManager->CreateUI<UI>();
    mpFridgeUI->LoadUIGraph("Resource/UI/Pause/Metallic Refrigerator.png");
    mpFridgeUI->GetTransform().SetPosition(VGet(0.0f, 30.0f, 0.0f));
    mpFridgeUI->GetTransform().SetScale(VGet(2.4f, 2.4f, 0.0f));
    mpFridgeUI->SetLayer(2);        // 付箋(3)より下
    mpFridgeUI->GetText().SetFontId(FontId::Title);
    mpFridgeUI->GetText().SetOffset(VGet(250.0f, -350.0f, 0.0f));
    mpFridgeUI->GetText().SetText("Pause");
    mpFridgeUI->SetLayer(2);
    mpFridgeUI->SetEnabled(true);
    mpMenuRoot->AddChild(mpFridgeUI);
    mpFridgeUI->SetParent(mpMenuRoot);

    // ★変更：4つのボタンを付箋に
    mpResumeUI = CreateStickyNote({
        "Resource/UI/Pause/Gray sticky notes.png",  "再開",
        VGet(-324.0f, -243.0f, 0.0f), 0.75f,  0.21f });

    mpStageSelectUI = CreateStickyNote({
        "Resource/UI/Pause/Orange Sticky Notes.png", "ステージ選択シーンへ",
        VGet(234.0f, -19.0f, 0.0f),   0.70f, -0.08f });

    mpTitleUI = CreateStickyNote({
        "Resource/UI/Pause/Green Sticky Note.png",  "タイトルへ",
        VGet(-387.0f, 262.0f, 0.0f),  0.59f, -0.02f });

    mpOperationButton = CreateStickyNote({
        "Resource/UI/Pause/Pink Sticky Notes.png",  "操作説明",
        VGet(307.0f, 368.0f, 0.0f),   0.60f,  0.07f });

    // 操作説明（変更なし）
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

UI* PauseUI::CreateStickyNote(const StickyNoteDesc& _desc)
{
    UI* note = GetUIManager()->CreateUI<UI>();
    note->LoadUIGraph(_desc.graphPath);

    auto& transform = note->GetTransform();
    transform.SetPosition(_desc.position);
    transform.SetScale(VGet(_desc.scale, _desc.scale, 0.0f));
    transform.SetRotation(VGet(0.0f, 0.0f, _desc.rotation));

    auto& text = note->GetText();
    text.SetFontId(FontId::SubTitle);
    text.SetText(_desc.text);
    text.SetColor(GetColor(50, 35, 25));        // 明るい付箋でも読めるよう濃い茶色
    text.SetEdgeColor(GetColor(255, 255, 255)); // 縁付きフォントのときだけ有効
    text.SetOffset(VGet(0.0f, 15.0f, 0.0f));    // 上のピンを避けて少し下げる

    note->GetHoverScaler().SetActive(true);     // マウスを乗せると少し大きくなる（任意）

    note->SetLayer(3);
    note->SetEnabled(true);
    mpMenuRoot->AddChild(note);
    note->SetParent(mpMenuRoot);
    return note;
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

    // -- 制限中でない場合のみ、他シーンへの遷移ボタンを有効にする -- //
    if (!mbResumeOnly)
    {
        // -- 「ステージ選択シーンへ」を選択した場合 -- //
        if (mpStageSelectUI != nullptr &&
            mpStageSelectUI->GetButton().IsClicked())
        {
            SoundManager::GetInstance()->StopBGM();
            SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title);
            Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
        }

        // -- 「タイトルへ」ボタンを選択した場合 -- //
        // ★ここも必ず if (!mbResumeOnly) の { } の中に入れます！
        if (mpTitleUI != nullptr &&
            mpTitleUI->GetButton().IsClicked())
        {
            SoundManager::GetInstance()->StopBGM();
            Master::mpSceneManager->RequestScene(SCENE_TYPE::TITLE_SCENE);
        }

    }
}

void PauseUI::UpdateMenu()
{
    if (mpResumeUI->GetButton().IsClicked())
    {
        SetEnabled(false);
        return;
    }
    if (mbResumeOnly)   // ★再開以外は押せない
        return;

    if (mpStageSelectUI->GetButton().IsClicked())
    {
        SoundManager::GetInstance()->StopBGM();
        SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title);
        Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
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