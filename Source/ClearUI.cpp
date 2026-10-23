#include "ClearUI.h"

#include "ClearCollectibleUI.h"

void ClearUI::Init()
{
    SetEnabled(false);

    mClearTextMotion = FloatingMotion(120, 5.0f);
}

void ClearUI::Update()
{
    mfItemTimer += 1.0f / 60.0f;

    mClearTextMotion.Update();

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
}

void ClearUI::SetCollectibleUI(
    int _index,
    ClearCollectibleUI* _ui
)
{
    if (_index < 0 || _index >= 3)
    {
        return;
    }

    mpCollectibleUI[_index] = _ui;
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
        GetColor(0, 0, 0),
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
        400,
        180,
        880,
        600,
        GetColor(80, 65, 45),
        TRUE
    );

    // 内側
    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        210
    );

    DrawBox(
        410,
        190,
        870,
        590,
        GetColor(245, 230, 190),
        TRUE
    );

    // 中央部分
    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        230
    );

    DrawBox(
        425,
        205,
        855,
        575,
        GetColor(45, 40, 35),
        TRUE
    );

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0
    );

    // 上側のアクセントライン
    DrawBox(
        425,
        205,
        855,
        215,
        GetColor(210, 170, 90),
        TRUE
    );

    // 下側のアクセントライン
    DrawBox(
        425,
        565,
        855,
        575,
        GetColor(210, 170, 90),
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
        GetColor(255, 255, 255)
    );

    // ステージ選択ボタン

    DrawBox(
        500,
        450,
        780,
        520,
        GetColor(80, 80, 80),
        TRUE
    );

    DrawBox(
        500,
        450,
        780,
        520,
        GetColor(255, 255, 255),
        FALSE
    );

    DrawString(
        570,
        475,
        "ステージ選択へ戻る",
        GetColor(255, 255, 255)
    );

    // アイテム

    for (int i = 0; i < 3; i++)
    {
        if (mpCollectibleUI[i] == nullptr)
        {
            continue;
        }

        // アイテムごとに少し時間差をつける
        float startTime = i * 0.15f;

        if (mfItemTimer < startTime)
        {
            continue;
        }

        float animationTime =
            mfItemTimer - startTime;

        const float animationDuration = 0.35f;

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
                GetColor(255, 255, 255)
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
                GetColor(160, 160, 160)
            );

            SetDrawBlendMode(
                DX_BLENDMODE_NOBLEND,
                0
            );
        }
    }
}