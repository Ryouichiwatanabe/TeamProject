#include "EffectClick.h"

EffectClick::EffectClick(float fX, float fY)
    : EffectBase(fX, fY, DURATION) // 寿命を基底クラスに渡すだけ
{
}

void EffectClick::OnUpdate(float fDeltaTime)
{
    // GetProgress() は 0.0f ～ 1.0f を返す
    const float progress = GetProgress();

    // 進行度に応じて半径を広げ、アルファを薄くするだけ
    m_fCurrentRadius = MAX_RADIUS * progress;
    m_fAlpha = 255.0f * (1.0f - progress);
}

void EffectClick::Draw() const
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(m_fAlpha));
    DrawCircle(static_cast<int>(m_fX), static_cast<int>(m_fY), static_cast<int>(m_fCurrentRadius), GetColor(255, 255, 255), FALSE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}