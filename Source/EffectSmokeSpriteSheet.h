#pragma once
#include "DxLib.h"
#include "EffectBase.h"

struct DivGraphData;

class EffectSmokeSpriteSheet : public EffectBase
{
public:
    // bIsLoop: true にすると消滅せずループ再生し続ける
    EffectSmokeSpriteSheet(
        float fX,
        float fY,
        const DivGraphData* pData,
        float fFrameDuration = 0.05f,
        bool bIsLoop = false,
        bool bIsAddBlend = false,
        double dScale = 1.0,
        double dAngle = 0.0
    );

    ~EffectSmokeSpriteSheet() override = default;

    void Draw() const override;

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    const DivGraphData* m_pDivData = nullptr;
    float               m_fFrameDuration = 0.05f;
    bool                m_bIsLoop = false; // ループ再生フラグ
    bool                m_bIsAddBlend = false;
    double              m_dScale = 1.0;
    double              m_dAngle = 0.0;
};