#include "PauseUI.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIManager.h"

void PauseUI::Init()
{
    SetEnabled(false);

    auto* uiManager =
        Master::mpSceneManager
        ->GetCurrentScene()
        ->GetUIManager();

    // 再開ボタン
    mpResumeUI = uiManager->CreateUI<UI>();

    mpResumeUI->LoadUIGraph(
        "Resource/UI/PauseBoard.png"
    );

    mpResumeUI->SetLayer(2);

    mpResumeUI->GetTransform().SetAnchor(
        VGet(0.0f, 0.0f, 0.0f)
    );

    mpResumeUI->GetTransform().SetPosition(
        VGet(
            640.0f,
            335.0f,
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

    mpResumeUI->GetText().SetText("再開");
    mpResumeUI->GetText().SetColor(GetColor(0, 0, 0));
    mpResumeUI->GetText().SetOffset(VGet(0.0f, 5.0f, 0.0f));

    AddChild(mpResumeUI);
    mpResumeUI->SetParent(this);
    mpResumeUI->SetEnabled(true);

    // ステージ選択
    mpStageSelectUI = uiManager->CreateUI<UI>();

    mpStageSelectUI->LoadUIGraph(
        "Resource/UI/PauseBoard.png"
    );

    mpStageSelectUI->SetLayer(2);

    mpStageSelectUI->GetTransform().SetAnchor(
        VGet(0.0f, 0.0f, 0.0f)
    );

    mpStageSelectUI->GetTransform().SetPosition(
        VGet(640.0f, 435.0f, 0.0f)
    );

    mpStageSelectUI->GetTransform().SetSize(
        VGet(
            280.0f,
            70.0f,
            0.0f
        )
    );

    mpStageSelectUI->GetText().SetText(
        "ステージ選択に戻る"
    );

    mpStageSelectUI->GetText().SetColor(
        GetColor(0, 0, 0)
    );

    mpStageSelectUI->GetText().SetOffset(
        VGet(-35.0f, 5.0f, 0.0f)
    );

    AddChild(mpStageSelectUI);
    mpStageSelectUI->SetParent(this);
    mpStageSelectUI->SetEnabled(true);

    // タイトルへ戻る
    mpTitleUI = uiManager->CreateUI<UI>();

    mpTitleUI->LoadUIGraph(
        "Resource/UI/PauseBoard.png"
    );

    mpTitleUI->SetLayer(2);

    mpTitleUI->GetTransform().SetAnchor(
        VGet(0.0f, 0.0f, 0.0f)
    );

    mpTitleUI->GetTransform().SetPosition(
        VGet(640.0f, 535.0f, 0.0f)
    );

    mpTitleUI->GetTransform().SetSize(
        VGet(
            280.0f,
            70.0f,
            0.0f
        )
    );

    mpTitleUI->GetText().SetText(
        "タイトルへ戻る"
    );

    mpTitleUI->GetText().SetColor(
        GetColor(0, 0, 0)
    );

    mpTitleUI->GetText().SetOffset(
        VGet(-20.0f, 5.0f, 0.0f)
    );

    AddChild(mpTitleUI);
    mpTitleUI->SetParent(this);
    mpTitleUI->SetEnabled(true);
}

void PauseUI::Update()
{
    UI::Update();

    if (mpResumeUI != nullptr &&
        mpResumeUI->GetButton().IsClicked())
    {
        mbIsResumeRequested = true;
        SetEnabled(false);
    }

    if (mpStageSelectUI != nullptr &&
        mpStageSelectUI->GetButton().IsClicked())
    {
        Master::mpSceneManager->RequestScene(
            SCENE_TYPE::STAGE_SELECT_SCENE
        );
    }

    if (mpTitleUI != nullptr &&
        mpTitleUI->GetButton().IsClicked())
    {
        Master::mpSceneManager->RequestScene(
            SCENE_TYPE::TITLE_SCENE
        );
    }
}

void PauseUI::Draw()
{
    // 暗転
    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        150
    );

    DrawBox(
        0,
        0,
        1280,
        720,
        GetColor(0, 0, 0),
        TRUE
    );

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0
    );

    // Pauseタイトル
    DrawStringToHandle(
        570,
        200,
        "Pause",
        GetColor(255, 255, 255),
        mTitleFontHandle
    );

    UI::Draw();
}

void PauseUI::SetTitleFontHandle(int _fontHandle)
{
    mTitleFontHandle = _fontHandle;
}

void PauseUI::SetMessageFontHandle(int _fontHandle)
{
    mMessageFontHandle = _fontHandle;

    if (mpResumeUI != nullptr)
    {
        mpResumeUI->GetText().SetFontHandle(_fontHandle);
    }

    if (mpStageSelectUI != nullptr)
    {
        mpStageSelectUI->GetText().SetFontHandle(_fontHandle);
    }

    if (mpTitleUI != nullptr)
    {
        mpTitleUI->GetText().SetFontHandle(_fontHandle);
    }
}