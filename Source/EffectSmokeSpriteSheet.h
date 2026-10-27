#pragma once
#include "EffectBase.h"
#include <algorithm>

// 前方宣言（定義元ヘッダーを直接 include しても構いません）
struct DivGraphData;

class EffectSmokeSpriteSheet : public EffectBase
{
public:
    EffectSmokeSpriteSheet(
        float fX,
        float fY,
        const DivGraphData* pData,
        float fFrameDuration,
        bool bIsLoop = false,
        bool bIsAddBlend = false,
        double dScale = 1.0,
        double dAngle = 0.0,
        int nAlpha = 255,
        int nRed = 255,
        int nGreen = 255,
        int nBlue = 255
    );

    virtual ~EffectSmokeSpriteSheet() override = default;

    virtual void OnUpdate(float fDeltaTime) override;
    virtual void Draw() const override;

    void SetAlpha(int nAlpha) { m_nAlpha = std::clamp(nAlpha, 0, 255); }
    void SetColor(int nRed, int nGreen, int nBlue)
    {
        m_nRed = std::clamp(nRed, 0, 255);
        m_nGreen = std::clamp(nGreen, 0, 255);
        m_nBlue = std::clamp(nBlue, 0, 255);
    }

private:
    const DivGraphData* m_pDivData{ nullptr }; // ここが認識されていないと m_pDivData 関連のエラーになる
    float  m_fFrameDuration{ 0.1f };
    bool   m_bIsLoop{ false };
    bool   m_bIsAddBlend{ false };
    double m_dScale{ 1.0 };
    double m_dAngle{ 0.0 };

    int m_nAlpha{ 255 };
    int m_nRed{ 255 };
    int m_nGreen{ 255 };
    int m_nBlue{ 255 };
};