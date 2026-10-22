#include "EffectClear.h"
#include "Master.h"
#include "ResourceManager.h"
#include <cmath>

EffectClear::EffectClear(float fCenterX, float fCenterY)
    : EffectBase(fCenterX, fCenterY, DURATION)
{
    m_nGraphHandle = Master::mpResource->LoadGraphics(std::string(TEXTURE_PATH));

    if (m_nGraphHandle != -1)
    {
        int sizeX = 0, sizeY = 0;
        GetGraphSize(m_nGraphHandle, &sizeX, &sizeY);
        m_fImageWidth = static_cast<float>(sizeX);
        m_fImageHeight = static_cast<float>(sizeY);
    }

    m_sparks.reserve(OUTER_COUNT + INNER_COUNT);

    // 華やかな花火カラーパレット
    const COLOR_U8 palette[] = {
        GetColorU8(255, 220, 100, 255), // ゴールド
        GetColorU8(255, 120, 180, 255), // ピンク・マゼンタ
        GetColorU8(100, 240, 255, 255), // シアン
        GetColorU8(255, 255, 255, 255), // 純白
        GetColorU8(255, 180, 50, 255)   // オレンジ
    };

    auto createSparks = [&](int count, float initRadius, float baseSpeed, float scaleMin, float scaleMax) {
        for (int i = 0; i < count; ++i)
        {
            const float baseAngle = (DX_TWO_PI_F / static_cast<float>(count)) * static_cast<float>(i);
            const float jitter = (static_cast<float>(GetRand(24) - 12) / 100.0f) * (DX_PI_F / 180.0f);

            RingSpark spark;
            spark.fAngle = baseAngle + jitter;
            spark.fDistance = initRadius + static_cast<float>(GetRand(20) - 10);
            spark.fSpeed = baseSpeed + static_cast<float>(GetRand(80) - 40);
            spark.fScale = (static_cast<float>(GetRand(10)) / 10.0f) * (scaleMax - scaleMin) + scaleMin;
            spark.fRotation = static_cast<float>(GetRand(360)) * (DX_PI_F / 180.0f);
            spark.fRotSpeed = static_cast<float>(GetRand(400) - 200) / 100.0f;
            spark.fFlickerOffset = static_cast<float>(GetRand(100)) / 10.0f;
            spark.color = palette[GetRand(4)];

            m_sparks.emplace_back(spark);
        }
        };

    // ① 外側リング：大粒で高速に拡散（外殻）
    createSparks(OUTER_COUNT, 160.0f, 260.0f, 1.0f, 1.4f);

    // ② 内側リング：中粒でやや遅れて広がる（厚みを持たせる層）
    createSparks(INNER_COUNT, 90.0f, 150.0f, 0.6f, 0.9f);
}

void EffectClear::OnUpdate(float fDeltaTime)
{
    for (auto& spark : m_sparks)
    {
        // 現在のワールド座標を記録（トレイル用）
        const float curX = m_fX + std::cos(spark.fAngle) * spark.fDistance;
        const float curY = m_fY + std::sin(spark.fAngle) * spark.fDistance;

        spark.trailPositions.push_front({ curX, curY });
        if (spark.trailPositions.size() > 4) // 最大4フレーム分の軌跡
        {
            spark.trailPositions.pop_back();
        }

        // 拡散物理更新（空気抵抗で緩やかに減速）
        spark.fDistance += spark.fSpeed * fDeltaTime;
        spark.fSpeed *= 0.94f;
        spark.fRotation += spark.fRotSpeed * fDeltaTime;
    }
}

void EffectClear::Draw() const
{
    const float fProgress = GetProgress(); // 0.0f ～ 1.0f

    // --- 1. 開幕の画面中央フラッシュ（最初の0.2秒間だけ光が弾ける） --- //
    if (fProgress < 0.2f)
    {
        const float flashRatio = 1.0f - (fProgress / 0.2f);
        SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(flashRatio * 200.0f));
        // 中心の閃光リング
        DrawCircle(
            static_cast<int>(m_fX),
            static_cast<int>(m_fY),
            static_cast<int>(300.0f * (fProgress / 0.2f)),
            GetColor(255, 240, 200),
            FALSE
        );
    }

    if (m_nGraphHandle == -1)
    {
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        return;
    }

    const float baseAlpha = 255.0f * (1.0f - (fProgress * fProgress));
    const float fCenterX = m_fImageWidth / 2.0f;
    const float fCenterY = m_fImageHeight / 2.0f;

    for (const auto& spark : m_sparks)
    {
        // 花火特有の高速な明滅
        const float flicker = 0.75f + 0.25f * std::sin((m_fElapsedTime * 30.0f) + spark.fFlickerOffset);
        const int currentAlpha = static_cast<int>(baseAlpha * flicker);

        if (currentAlpha <= 0) continue;

        // 色味を反映（SetDrawBright）
        SetDrawBright(spark.color.r, spark.color.g, spark.color.b);

        // --- 2. 残像（トレイル）の描画 --- //
        float trailScaleFactor = 0.7f;
        float trailAlphaFactor = 0.4f;

        for (const auto& [tx, ty] : spark.trailPositions)
        {
            SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(currentAlpha * trailAlphaFactor));

            DrawRotaGraph3(
                static_cast<int>(tx),
                static_cast<int>(ty),
                static_cast<int>(fCenterX),
                static_cast<int>(fCenterY),
                spark.fScale * trailScaleFactor,
                spark.fScale * trailScaleFactor,
                static_cast<double>(spark.fRotation),
                m_nGraphHandle,
                TRUE,
                FALSE
            );

            trailScaleFactor *= 0.8f;
            trailAlphaFactor *= 0.5f;
        }

        // --- 3. パーティクル本体の描画 --- //
        const float posX = m_fX + std::cos(spark.fAngle) * spark.fDistance;
        const float posY = m_fY + std::sin(spark.fAngle) * spark.fDistance;
        const float currentScale = spark.fScale * (1.0f - fProgress * 0.25f);

        SetDrawBlendMode(DX_BLENDMODE_ADD, currentAlpha);

        DrawRotaGraph3(
            static_cast<int>(posX),
            static_cast<int>(posY),
            static_cast<int>(fCenterX),
            static_cast<int>(fCenterY),
            currentScale,
            currentScale,
            static_cast<double>(spark.fRotation),
            m_nGraphHandle,
            TRUE,
            FALSE
        );
    }

    // 描画設定のリセット
    SetDrawBright(255, 255, 255);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}