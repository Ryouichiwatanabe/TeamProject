#pragma once
#include "EffectBase.h"
#include <vector>

class EffectFall final : public EffectBase
{
public:
    // fX, fY : 発生元の辺の中心座標
    // fOutwardAngleRad : 外側へ向かう角度（ラジアン）
    EffectFall(float fX, float fY, float fOutwardAngleRad = -1.570796f);
    ~EffectFall() override = default;

    void Draw() const override;

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    // ドット絵風の水飛沫1粒ごとの状態
    struct SplashDrop
    {
        float fX = 0.0f;
        float fY = 0.0f;
        float fVelocityX = 0.0f;
        float fVelocityY = 0.0f;
        int   nPixelSize = 6;     // ドット1粒のサイズ（4px, 6px, 8pxなど）
        bool  bIsWhite = false;   // 白ハイライトか水色か
    };

    std::vector<SplashDrop> m_drops;

    static constexpr float EFFECT_LIFETIME = 0.35f; // パッと弾けて消える秒数
    static constexpr int   DROP_COUNT = 7;     // 1つの外辺から出るドット粒の数
    static constexpr float GRAVITY = 750.0f;// 重力加速度 (px/s^2)
};