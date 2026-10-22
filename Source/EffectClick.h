#pragma once
#include "EffectBase.h"

class EffectClick final : public EffectBase
{
public:
    EffectClick(float fX, float fY);
    ~EffectClick() override = default;

    void Draw() const override;

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    float m_fCurrentRadius = 0.0f;
    float m_fAlpha = 255.0f;

    static constexpr float MAX_RADIUS = 50.0f;
    static constexpr float DURATION = 0.3f; // 0.3ïbÇ≈è¡Ç¶ÇÈ
};