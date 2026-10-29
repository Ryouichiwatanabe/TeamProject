#include "ClearUI.h"
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIManager.h"
#include "ClearCollectibleUI.h"


ClearUI::ClearUI(
    const std::string& _filename0,
    const std::string& _filename1,
    const std::string& _filename2
)
{
    mCollectibleFilename[0] = _filename0;
    mCollectibleFilename[1] = _filename1;
    mCollectibleFilename[2] = _filename2;
}

void ClearUI::Init()
{
    SetEnabled(false);

    mMessageFontHandle =
        CreateFontToHandle(
            "PixelMplus10",
            20,
            -1,
            DX_FONTTYPE_NORMAL
        );

    mClearTextMotion = FloatingMotion(120, 5.0f);

    auto* uiManager =
        Master::mpSceneManager
        ->GetCurrentScene()
        ->GetUIManager();

    // クリアアイテム

    for (int i = 0; i < 3; i++)
    {
        mpCollectibleUI[i] =
            uiManager->CreateUI<ClearCollectibleUI>();

        mpCollectibleUI[i]->SetGraph(
            mCollectibleFilename[i]
        );

        mpCollectibleUI[i]
            ->GetTransform()
            .SetAnchor(
                VGet(0.0f, 0.0f, 0.0f)
            );

        mpCollectibleUI[i]
            ->GetTransform()
            .SetPosition(
                VGet(
                    550.0f + i * 90.0f,
                    315.0f,
                    0.0f
                )
            );

        mpCollectibleUI[i]
            ->GetTransform()
            .SetScale(
                VGet(
                    0.5f,
                    0.5f,
                    1.0f
                )
            );

        // ClearUIの子UIにする
        AddChild(mpCollectibleUI[i]);
        mpCollectibleUI[i]->SetParent(this);

        // 最初は非表示
        mpCollectibleUI[i]->SetEnabled(false);
    }

    // ステージ選択へ戻るボタン
    mpStageSelectUI =
        uiManager->CreateUI<UI>();

    mpStageSelectUI->SetLayer(2);

    mpStageSelectUI
        ->LoadUIGraph(
            "Resource/UI/PauseBoard.png"
        );

    mpStageSelectUI
        ->GetTransform()
        .SetAnchor(
            VGet(0.0f, 0.0f, 0.0f)
        );

    mpStageSelectUI
        ->GetTransform()
        .SetPosition(
            VGet(
                650.0f,
                485.0f,
                0.0f
            )
        );

    mpStageSelectUI
        ->GetTransform()
        .SetScale(
            VGet(
                0.8f,
                0.8f,
                1.0f
            )
        );

    mpStageSelectUI
        ->GetText()
        .SetText(
            "ステージ選択へ戻る"
        );

    mpStageSelectUI
        ->GetText()
        .SetFontHandle(
            mMessageFontHandle
        );

    mpStageSelectUI
        ->GetText()
        .SetColor(
            GetColor(
                0,
                0,
                0
            )
        );

    mpStageSelectUI
        ->GetText()
        .SetOffset(
            VGet(
                -10.0f,
                0.0f,
                0.0f
            )
        );

    // ClearUIの子UIにする
    AddChild(mpStageSelectUI);
    mpStageSelectUI->SetParent(this);

    mpStageSelectUI->SetEnabled(true);
}


void ClearUI::Update()
{
    // アイテム出現アニメーション用タイマー
    mfItemTimer += 1.0f / 60.0f;

    // CLEAR! の上下アニメーション
    mClearTextMotion.Update();

    // アイテムの獲得状態を反映

    for (int i = 0; i < 3; i++)
    {
        if (mpCollectibleUI[i] == nullptr)
        {
            continue;
        }

        if (mbIsCollected[i])
        {
            mpCollectibleUI[i]->SetCollected(true);
        }
    }

    // ステージ選択へ戻る

    if (mpStageSelectUI != nullptr &&
        mpStageSelectUI->GetButton().IsClicked())
    {
        Master::mpSceneManager->RequestScene(
            SCENE_TYPE::STAGE_SELECT_SCENE
        );
    }
}


void ClearUI::SetCollected(
    int _index,
    bool _isCollected
)
{
    if (_index < 0 || _index >= 3)
    {
        return;
    }

    mbIsCollected[_index] = _isCollected;
}


void ClearUI::Draw()
{
    // 暗転

    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        180
    );

    DrawBox(
        0,
        0,
        1280,
        720,
        GetColor(
            0,
            0,
            0
        ),
        TRUE
    );

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0
    );

    // クリアパネル

    // 外側
    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        220
    );

    DrawBox(
        400,180,
        880,600,
   GetColor(
            80,65,45
        ),
        TRUE
    );


    // 内側
    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        210
    );

    DrawBox(
        410,190,
        870,590,
   GetColor(
            245,230,190
        ),
        TRUE
    );


    // 中央部分
    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        230
    );

    DrawBox(
        425,205,
        855,575,
   GetColor(
            45,40,35
        ),
        TRUE
    );

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0
    );

    // 上側のアクセントライン

    DrawBox(
        425,205,
        855,215,
   GetColor(
            210,170,90
        ),
        TRUE
    );

    // 下側のアクセントライン

    DrawBox(
        425,565,
        855,575,
   GetColor(
            210,170,90
        ),
        TRUE
    );

    // CLEAR!

    int clearOffsetY =
        static_cast<int>(
            mClearTextMotion.GetOffsetY()
            );

    DrawString(
        620,
        220 + clearOffsetY,
        "CLEAR!",
        GetColor(255,255,255
    )
   );

    // ボタン文字はmpStageSelectUIのUITextで描画する
    // DrawStringは重複するため使用しない

    // アイテム

    for (int i = 0; i < 3; i++)
    {
        if (mpCollectibleUI[i] == nullptr)
        {
            continue;
        }

        // アイテムごとに少し時間差をつける

        float startTime =
            i * 0.15f;

        if (mfItemTimer < startTime)
        {
            continue;
        }

        // アニメーション時間

        float animationTime =
            mfItemTimer - startTime;

        const float animationDuration =
            0.35f;

        // 0.0 ～ 1.0

        float rate =
            animationTime / animationDuration;

        if (rate > 1.0f)
        {
            rate = 1.0f;
        }

        // 小さい状態から元の大きさへ

        float scale =
            0.2f + 0.3f * rate;


        mpCollectibleUI[i]
            ->GetTransform()
            .SetScale(
                VGet(
                    scale,
                    scale,
                    1.0f
                )
            );


        mpCollectibleUI[i]->Draw();

        // 獲得状態

        if (mbIsCollected[i])
        {
            DrawString(
                530 + i * 90,
                365,
                "獲得",
                GetColor(
                    255,
                    255,
                    255
                )
            );
        }
        else
        {
            SetDrawBlendMode(
                DX_BLENDMODE_ALPHA,
                150
            );

            DrawString(
                530 + i * 90,
                365,
                "未獲得",
                GetColor(
                    160,
                    160,
                    160
                )
            );

            SetDrawBlendMode(
                DX_BLENDMODE_NOBLEND,
                0
            );
        }
    }
}

ClearUI::~ClearUI()
{
    if (mMessageFontHandle != -1)
    {
        DeleteFontToHandle(mMessageFontHandle);
        mMessageFontHandle = -1;
    }
}