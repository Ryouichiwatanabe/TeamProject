#include "EffectFall.h"
#include <random>
#include <numbers>
#include <cmath>

EffectFall::EffectFall(float fX, float fY)
    : EffectBase(fX, fY, EFFECT_LIFETIME)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // 豆腐の左右・上方向にバシャッと広がる角度 (-155度 ? -25度)
    std::uniform_real_distribution<float> distAngle(-155.0f, -25.0f);
    // 初速のばらつき (px/s)
    std::uniform_real_distribution<float> distSpeed(140.0f, 320.0f);
    // 水滴サイズのばらつき
    std::uniform_real_distribution<float> distRadius(2.0f, 4.5f);

    m_drops.reserve(DROP_COUNT);

    for (int i = 0; i < DROP_COUNT; ++i)
    {
        const float fAngleRad = distAngle(gen) * (std::numbers::pi_v<float> / 180.0f);
        const float fSpeed = distSpeed(gen);

        SplashDrop drop;
        drop.fX = m_fX;
        drop.fY = m_fY;
        drop.fVelocityX = std::cos(fAngleRad) * fSpeed;
        drop.fVelocityY = std::sin(fAngleRad) * fSpeed;
        drop.fBaseRadius = distRadius(gen);

        m_drops.emplace_back(drop);
    }
}

void EffectFall::OnUpdate(float fDeltaTime)
{
    // 各水滴の放物線運動の更新
    for (auto& drop : m_drops)
    {
        drop.fX += drop.fVelocityX * fDeltaTime;
        drop.fY += drop.fVelocityY * fDeltaTime;
        drop.fVelocityY += GRAVITY * fDeltaTime; // 重力を加算
    }
}

void EffectFall::Draw() const
{
    const float fProgress = GetProgress(); // 0.0f (発生時) -> 1.0f (終了時)

    // 後半にかけてフェードアウト（透明度計算）
    const float fAlphaRate = 1.0f - fProgress;
    const int nAlpha = static_cast<int>(255.0f * fAlphaRate);
    if (nAlpha <= 0) return;

    // 清涼感のある水色
    const unsigned int colorWater = GetColor(180, 225, 255);

    // --- 1. 着地点の波紋（床に広がる楕円） ---
    // 波紋はイージングアウト（急速に広がってゆっくり止まる）
    const float fRippleScale = std::sin(fProgress * (std::numbers::pi_v<float> *0.5f));
    const int nRadiusX = static_cast<int>(MAX_RIPPLE_X * fRippleScale);
    const int nRadiusY = static_cast<int>(MAX_RIPPLE_Y * fRippleScale);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, nAlpha);

    if (nRadiusX > 0 && nRadiusY > 0)
    {
        DrawOval(
            static_cast<int>(m_fX),
            static_cast<int>(m_fY),
            nRadiusX,
            nRadiusY,
            colorWater,
            FALSE // 線描画（輪）
        );
    }

    // --- 2. 飛び散る水滴 ---
    for (const auto& drop : m_drops)
    {
        // 飛び散りながら水滴自体も少しずつ小さくする
        const float fCurrentRadius = drop.fBaseRadius * fAlphaRate;
        if (fCurrentRadius < 0.5f) continue;

        DrawCircle(
            static_cast<int>(drop.fX),
            static_cast<int>(drop.fY),
            static_cast<int>(fCurrentRadius),
            colorWater,
            TRUE // 塗りつぶし
        );
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}