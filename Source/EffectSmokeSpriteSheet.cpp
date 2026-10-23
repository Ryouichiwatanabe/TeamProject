#include "EffectSmokeSpriteSheet.h"
#include "ResourceManager.h"
#include <algorithm>

EffectSmokeSpriteSheet::EffectSmokeSpriteSheet(
    float fX,
    float fY,
    const DivGraphData* pData,
    float fFrameDuration,
    bool bIsLoop,
    bool bIsAddBlend,
    double dScale,
    double dAngle
)
// ループ時は寿命を 0.0f に設定（EffectBase仕様により時間経過で自動消滅しない）
// ループしない場合は「総コマ数 * 1コマ秒数」を寿命にする
    : EffectBase(fX, fY, (bIsLoop || pData == nullptr) ? 0.0f : (static_cast<float>(pData->allNum) * fFrameDuration))
    , m_pDivData(pData)
    , m_fFrameDuration(fFrameDuration)
    , m_bIsLoop(bIsLoop)
    , m_bIsAddBlend(bIsAddBlend)
    , m_dScale(dScale)
    , m_dAngle(dAngle)
{
    if (!m_pDivData || m_pDivData->allNum <= 0)
    {
        m_bIsDead = true;
    }
}

void EffectSmokeSpriteSheet::OnUpdate([[maybe_unused]] float fDeltaTime)
{
}

void EffectSmokeSpriteSheet::Draw() const
{
    if (m_bIsDead || !m_pDivData) return;

    int currentIndex = 0;

    if (m_bIsLoop)
    {
        // ループ時はコマ数を剰余演算で巡回させる
        int totalFrames = static_cast<int>(m_fElapsedTime / m_fFrameDuration);
        currentIndex = totalFrames % m_pDivData->allNum;
    }
    else
    {
        // 単発再生時は最後のコマでクランプ
        const int maxIndex = m_pDivData->allNum - 1;
        currentIndex = std::clamp(
            static_cast<int>(m_fElapsedTime / m_fFrameDuration),
            0,
            maxIndex
        );
    }

    const int currentHandle = m_pDivData->divHandleList[currentIndex];
    if (currentHandle == -1) return;

    if (m_bIsAddBlend)
    {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    }

    DrawRotaGraphF(m_fX, m_fY, m_dScale, m_dAngle, currentHandle, TRUE);

    if (m_bIsAddBlend)
    {
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}