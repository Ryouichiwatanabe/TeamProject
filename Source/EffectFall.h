#pragma once
#include "EffectBase.h"
#include <vector>

class EffectFall final : public EffectBase
{
public:
    EffectFall(float fX, float fY);
    ~EffectFall() override = default;

    void Draw() const override;

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    // 水滴1粒ごとの状態（コンポジション）
    struct SplashDrop
    {
        float fX = 0.0f;
        float fY = 0.0f;
        float fVelocityX = 0.0f;
        float fVelocityY = 0.0f;
        float fBaseRadius = 3.0f;
    };

    std::vector<SplashDrop> m_drops;

    // パラメータ定数
    static constexpr float EFFECT_LIFETIME = 0.45f;    // エフェクトの生存時間（秒）
    static constexpr int   DROP_COUNT = 14;       // 飛び散る水滴の数
    static constexpr float GRAVITY = 850.0f;   // 重力加速度 (px/s^2)
    static constexpr float MAX_RIPPLE_X = 100.0f;    // 地面波紋の最大半径X
    static constexpr float MAX_RIPPLE_Y = 33.0f;    // 地面波紋の最大半径Y（楕円のつぶれ具合）
};