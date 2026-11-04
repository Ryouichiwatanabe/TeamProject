#pragma once
#include "EffectBase.h"
#include <algorithm>

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
        int nBlue = 255,
        float fPivotX = -1.0f, // 負の値の場合は画像中心を使用
        float fPivotY = -1.0f
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

    // コマ内の回転・配置基準点（ローカル座標）を設定
    void SetPivot(float fPivotX, float fPivotY)
    {
        m_fPivotX = fPivotX;
        m_fPivotY = fPivotY;
    }

private:
    const DivGraphData* m_pDivData{ nullptr };
    float  m_fFrameDuration{ 0.1f };
    bool   m_bIsLoop{ false };
    bool   m_bIsAddBlend{ false };
    double m_dScale{ 1.0 };
    double m_dAngle{ 0.0 };

    int m_nAlpha{ 255 };
    int m_nRed{ 255 };
    int m_nGreen{ 255 };
    int m_nBlue{ 255 };

    // ピボット（コマ内基準座標）。負値なら画像中央
    float m_fPivotX{ -1.0f };
    float m_fPivotY{ -1.0f };
};