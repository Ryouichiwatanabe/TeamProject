#include "PauseUI.h"

#include "Master.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "Scene.h"
#include "UIManager.h"

void PauseUI::Init()
{
    // -- 自分の初期処理 -- //
    SetEnabled(false);
    this->GetText().SetText("Pause");
    this->GetText().SetOffset(VGet(0.0f, -300.0f, 0.0f));

    auto* uiManager =
        Master::mpSceneManager
        ->GetCurrentScene()
        ->GetUIManager();

   // -- 再開ボタン -- //
    mpResumeUI = uiManager->CreateUI<UI>();
    mpResumeUI->LoadUIGraph("Resource/UI/PauseBoard.png");
    mpResumeUI->GetText().SetText("再開");

    mpResumeUI->SetLayer(2);
    mpResumeUI->GetTransform().SetPosition(
        VGet(
            0.0f,
            120.0f,
            0.0f
        )
    );
    mpResumeUI->GetTransform().SetSize(
        VGet(
            280.0f,
            70.0f,
            0.0f
        )
    );

    AddChild(mpResumeUI);
    mpResumeUI->SetParent(this);

    // -- ステージ選択画面へ戻るボタン -- //
    mpStageSelectUI = uiManager->CreateUI<UI>();
    mpStageSelectUI->LoadUIGraph("Resource/UI/PauseBoard.png");
    mpStageSelectUI->GetText().SetText("ステージ選択に戻る");

    mpStageSelectUI->SetLayer(2);
    mpStageSelectUI->GetTransform().SetPosition(
        VGet(
            0.0f,
            0.0f,
            0.0f
        )
    );
    mpStageSelectUI->GetTransform().SetSize(
        VGet(
            280.0f,
            70.0f,
            0.0f
        )
    );

    AddChild(mpStageSelectUI);
    mpStageSelectUI->SetParent(this);

    // -- タイトルへ戻るのボタン -- //
    mpTitleUI = uiManager->CreateUI<UI>();
    mpTitleUI->LoadUIGraph( "Resource/UI/PauseBoard.png");
    mpTitleUI->GetText().SetText("タイトルへ戻る");

    mpTitleUI->SetLayer(2);
    mpTitleUI->GetTransform().SetPosition(
        VGet(
            0.0f,
            -120.0f,
            0.0f
        )
    );
    mpTitleUI->GetTransform().SetSize(
        VGet(
            280.0f,
            70.0f,
            0.0f
        )
    );

    AddChild(mpTitleUI);
    mpTitleUI->SetParent(this);

    // -- オーバーレイ -- //
    mpPauseOverlay = uiManager->CreateUI<UI>();
    mpPauseOverlay->LoadUIGraph("Resource/UI/Pause/OverLay_Black.png");

    AddChild(mpPauseOverlay);
    mpPauseOverlay->SetParent(this);
}

void PauseUI::Update()
{
    UI::Update();

    // -- 「再開」ボタンを選択した場合 -- //
    if (mpResumeUI != nullptr &&
        mpResumeUI->GetButton().IsClicked())
    {
        SetEnabled(false);
    }

    // -- 「ステージ選択シーンへ」を選択した場合
    if (mpStageSelectUI != nullptr &&
        mpStageSelectUI->GetButton().IsClicked())
    {
        SoundManager::GetInstance()->StopBGM();
        SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title);
        Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);   
    }

    // -- 「タイトルへ」ボタンを選択した場合 -- //
    if (mpTitleUI != nullptr &&
        mpTitleUI->GetButton().IsClicked())
    {
        SoundManager::GetInstance()->StopBGM();
        Master::mpSceneManager->RequestScene(SCENE_TYPE::TITLE_SCENE);
    }

    if (IsEnabled()) {
        mpResumeUI->SetEnabled(true);
        mpStageSelectUI->SetEnabled(true);
        mpTitleUI->SetEnabled(true);
        mpPauseOverlay->SetEnabled(true);
    }

    else {
        mpResumeUI->SetEnabled(false);
        mpStageSelectUI->SetEnabled(false);
        mpTitleUI->SetEnabled(false);
        mpPauseOverlay->SetEnabled(false);
    }
}

void PauseUI::Draw()
{
    UI::Draw();
}