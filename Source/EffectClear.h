#pragma once

#include <vector>
#include <deque>
#include <string_view>
#include "EffectBase.h"

class EffectClear final : public EffectBase
{
public:
    EffectClear(float fCenterX, float fCenterY);
    ~EffectClear() override = default;

    void Draw() const override;

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    struct RingSpark
    {
        float fAngle = 0.0f;
        float fDistance = 0.0f;
        float fSpeed = 0.0f;
        float fScale = 1.0f;
        float fRotation = 0.0f;
        float fRotSpeed = 0.0f;
        float fFlickerOffset = 0.0f;
        COLOR_U8 color{ 255, 255, 255, 255 }; // 粒ごとの発光色

        // 残像（トレイル）用の過去座標（最大3〜4フレーム分）
        std::deque<std::pair<float, float>> trailPositions;
    };

    static constexpr std::string_view TEXTURE_PATH = "Resource/Clear.png";
    static constexpr float DURATION = 1.6f;
    static constexpr int   OUTER_COUNT = 48; // 外環の粒数
    static constexpr int   INNER_COUNT = 32; // 内環の粒数

    int   m_nGraphHandle = -1;
    float m_fImageWidth = 0.0f;
    float m_fImageHeight = 0.0f;

    std::vector<RingSpark> m_sparks;
};