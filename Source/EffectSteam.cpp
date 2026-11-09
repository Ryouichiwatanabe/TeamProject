#include "EffectSteam.h"
#include <random>
#include <cmath>

EffectSteam::EffectSteam(float fCenterX, float fCenterY)
    : EffectBase(fCenterX, fCenterY, INFINITE_LIFE_TIME)
{
    m_wisps.resize(WISP_COUNT);
    for (auto& wisp : m_wisps)
    {
        // ゲーム開始時から自然に湯気が立ち上っているよう初期経過時間をばらけさせる
        ResetWisp(wisp, true);
    }
}


void EffectSteam::ResetWisp(SteamWisp& wisp, bool bRandomInitialAge)
{
    static std::mt19937 gen(std::random_device{}());

    std::uniform_real_distribution<float> distX(-POT_SPAWN_RADIUS_X, POT_SPAWN_RADIUS_X);
    std::uniform_real_distribution<float> distY(POT_SPAWN_MIN_Y, POT_SPAWN_MAX_Y);
    std::uniform_real_distribution<float> distSpeed(MIN_RISE_SPEED, MAX_RISE_SPEED);
    std::uniform_real_distribution<float> distSwayAmp(MIN_SWAY_AMP, MAX_SWAY_AMP);
    std::uniform_real_distribution<float> distSwayFreq(MIN_SWAY_FREQ, MAX_SWAY_FREQ);
    std::uniform_real_distribution<float> distPhase(MIN_PHASE, MAX_PHASE);
    std::uniform_real_distribution<float> distWidth(MIN_BASE_WIDTH, MAX_BASE_WIDTH);
    std::uniform_real_distribution<float> distLife(MIN_LIFE_TIME, MAX_LIFE_TIME);

    wisp.fOriginX = m_fX + distX(gen);
    wisp.fCurrentY = m_fY + distY(gen);
    wisp.fRiseSpeed = distSpeed(gen);
    wisp.fSwayAmp = distSwayAmp(gen);
    wisp.fSwayFreq = distSwayFreq(gen);
    wisp.fPhase = distPhase(gen);
    wisp.fBaseWidth = distWidth(gen);
    wisp.fMaxLife = distLife(gen);

    if (bRandomInitialAge)
    {
        std::uniform_real_distribution<float> distAge(INITIAL_AGE_ZERO, wisp.fMaxLife);
        wisp.fAge = distAge(gen);
        wisp.fCurrentY -= wisp.fRiseSpeed * wisp.fAge;
    }
    else
    {
        wisp.fAge = INITIAL_AGE_ZERO;
    }
}

void EffectSteam::OnUpdate(float fDeltaTime)
{
    for (auto& wisp : m_wisps)
    {
        wisp.fAge += fDeltaTime;
        wisp.fCurrentY -= wisp.fRiseSpeed * fDeltaTime;

        if (wisp.fAge >= wisp.fMaxLife)
        {
            ResetWisp(wisp, false);
        }
    }
}

void EffectSteam::Draw() const
{
    const unsigned int steamColor = GetColor(STEAM_COLOR_R, STEAM_COLOR_G, STEAM_COLOR_B);
    const float centerNodeIndex = static_cast<float>(WISP_NODE_COUNT) * HALF_RATIO;

    for (const auto& wisp : m_wisps)
    {
        const float progress = wisp.fAge / wisp.fMaxLife;

        // 湧き始めはふわっと現れ、上に行くにつれてスーッと消える
        float lifeAlphaRate = INITIAL_AGE_ZERO;
        if (progress < FADE_IN_END_RATIO)
        {
            lifeAlphaRate = progress / FADE_IN_END_RATIO;
        }
        else
        {
            lifeAlphaRate = UNIT_RATIO - ((progress - FADE_IN_END_RATIO) / FADE_OUT_SPAN_RATIO);
        }

        // 上昇するにつれて少しずつ横に広がって薄くなる
        const float currentBaseRx = wisp.fBaseWidth * (UNIT_RATIO + progress * EXPAND_SCALE_RATE);

        // 縦に連なるノードをS字に揺らして「立ち上る湯気の筋」を描く
        for (int i = 0; i < WISP_NODE_COUNT; ++i)
        {
            const float nodeIndexF = static_cast<float>(i);

            // 筋の上端・下端ほど細く・薄くして自然な湯気の形にする
            const float distFromMid = std::fabsf(nodeIndexF - centerNodeIndex);
            const float nodeTaper = UNIT_RATIO - (distFromMid * NODE_TAPER_RATE);

            const int outerAlpha = static_cast<int>(MAX_OUTER_ALPHA * lifeAlphaRate * nodeTaper);
            const int coreAlpha = static_cast<int>(MAX_CORE_ALPHA * lifeAlphaRate * nodeTaper);
            if (outerAlpha <= MIN_VISIBLE_ALPHA)
            {
                continue;
            }

            // 高さごとに位相をずらすことで、下から上へS字にくねる動きを作る
            const float waveAngle = (wisp.fAge * wisp.fSwayFreq) + wisp.fPhase - (nodeIndexF * NODE_PHASE_DELAY);
            const float swayOffset = std::sinf(waveAngle) * wisp.fSwayAmp * (UNIT_RATIO + progress * HALF_RATIO);

            const int drawX = static_cast<int>(wisp.fOriginX + swayOffset);
            const int drawY = static_cast<int>(wisp.fCurrentY - (nodeIndexF * NODE_STEP_Y));

            const int coreRx = static_cast<int>(currentBaseRx * nodeTaper);
            const int coreRy = static_cast<int>(static_cast<float>(coreRx) * VERTICAL_STRETCH);
            const int outerRx = static_cast<int>(static_cast<float>(coreRx) * OUTER_RADIUS_RATE);
            const int outerRy = static_cast<int>(static_cast<float>(coreRy) * OUTER_RADIUS_RATE);

            // 1層目：外側のふんわり広がる霞
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, outerAlpha);
            DrawOval(drawX, drawY, outerRx, outerRy, steamColor, TRUE);

            // 2層目：内側の湯気の芯
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, coreAlpha);
            DrawOval(drawX, drawY, coreRx, coreRy, steamColor, TRUE);
        }
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NOBLEND_PARAM);
}