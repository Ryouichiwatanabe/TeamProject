#include "EffectSmokeSpriteSheet.h"
#include "ResourceManager.h"
#include <algorithm>
#include <DxLib.h>

EffectSmokeSpriteSheet::EffectSmokeSpriteSheet(
    float fX,
    float fY,
    const DivGraphData* pData,
    float fFrameDuration,
    bool bIsLoop,
    bool bIsAddBlend,
    double dScale,
    double dAngle,
    int nAlpha,
    int nRed,
    int nGreen,
    int nBlue
)
    : EffectBase(fX, fY, (bIsLoop || pData == nullptr) ? 0.0f : (static_cast<float>(pData->allNum) * fFrameDuration))
    , m_pDivData(pData)
    , m_fFrameDuration(fFrameDuration)
    , m_bIsLoop(bIsLoop)
    , m_bIsAddBlend(bIsAddBlend)
    , m_dScale(dScale)
    , m_dAngle(dAngle)
    , m_nAlpha(std::clamp(nAlpha, 0, 255))
    , m_nRed(std::clamp(nRed, 0, 255))
    , m_nGreen(std::clamp(nGreen, 0, 255))
    , m_nBlue(std::clamp(nBlue, 0, 255))
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

    // コマ番号の算出
    int currentIndex = 0;
    if (m_bIsLoop)
    {
        int totalFrames = static_cast<int>(m_fElapsedTime / m_fFrameDuration);
        currentIndex = totalFrames % m_pDivData->allNum;
    }
    else
    {
        const int maxIndex = m_pDivData->allNum - 1;
        currentIndex = std::clamp(
            static_cast<int>(m_fElapsedTime / m_fFrameDuration),
            0,
            maxIndex
        );
    }

    const int currentHandle = m_pDivData->divHandleList[currentIndex];
    if (currentHandle == -1) return;

    // 1. 描画輝度（カラーブレンド）の設定
    const bool bHasCustomColor = (m_nRed != 255 || m_nGreen != 255 || m_nBlue != 255);
    if (bHasCustomColor)
    {
        SetDrawBright(m_nRed, m_nGreen, m_nBlue);
    }

    // 2. ブレンドモードの設定（加算または半透明）
    const int blendMode = m_bIsAddBlend ? DX_BLENDMODE_ADD : DX_BLENDMODE_ALPHA;
    SetDrawBlendMode(blendMode, m_nAlpha);

    // 3. 描画
    DrawRotaGraphF(m_fX, m_fY, m_dScale, m_dAngle, currentHandle, TRUE);

    // 4. 描画ステートのリセット（描画ステート汚染の防止）
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    if (bHasCustomColor)
    {
        SetDrawBright(255, 255, 255);
    }
}