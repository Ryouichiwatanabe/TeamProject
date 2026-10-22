#include "EffectClick.h"
#include <DxLib.h>
#include <algorithm>

namespace
{
    // エフェクトの挙動パラメータ
    constexpr float kInitialRadius = 5.0f;
    constexpr float kExpandSpeed = 120.0f;  // 1秒間に広がる半径（ピクセル）
    constexpr float kFadeSpeed = 450.0f;    // 1秒間に減少するアルファ値
    constexpr int kCircleColor = 0xFFFFFF;  // 白色（RGB: 255, 255, 255）
}

EffectClick::EffectClick(float fX, float fY)
    : m_fX(fX)
    , m_fY(fY)
    , m_fCurrentRadius(kInitialRadius)
    , m_fAlpha(255.0f)
    , m_bIsDead(false)
{
}

void EffectClick::Update(float fDeltaTime)
{
    if (m_bIsDead)
    {
        return;
    }

    // 半径の拡大とアルファ値の減衰
    m_fCurrentRadius += kExpandSpeed * fDeltaTime;
    m_fAlpha -= kFadeSpeed * fDeltaTime;

    // 完全に透明になったら死亡フラグを立てる
    if (m_fAlpha <= 0.0f)
    {
        m_fAlpha = 0.0f;
        m_bIsDead = true;
    }
}

void EffectClick::Draw() const
{
    if (m_bIsDead)
    {
        return;
    }

    // アルファブレンドを有効化
    const int blendAlpha = std::clamp(static_cast<int>(m_fAlpha), 0, 255);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, blendAlpha);

    // 波紋の輪郭線を描画
    DrawCircle(
        static_cast<int>(m_fX),
        static_cast<int>(m_fY),
        static_cast<int>(m_fCurrentRadius),
        kCircleColor,
        FALSE // 塗りつぶしなし（枠線のみ）
    );

    // ブレンドモードを通常描画に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}