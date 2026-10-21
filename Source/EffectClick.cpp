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

EffectClick::EffectClick(float x, float y)
    : x_(x)
    , y_(y)
    , currentRadius_(kInitialRadius)
    , alpha_(255.0f)
    , isDead_(false)
{
}

void EffectClick::Update(float deltaTime)
{
    if (isDead_)
    {
        return;
    }

    // 半径の拡大とアルファ値の減衰
    currentRadius_ += kExpandSpeed * deltaTime;
    alpha_ -= kFadeSpeed * deltaTime;

    // 完全に透明になったら死亡フラグを立てる
    if (alpha_ <= 0.0f)
    {
        alpha_ = 0.0f;
        isDead_ = true;
    }
}

void EffectClick::Draw() const
{
    if (isDead_)
    {
        return;
    }

    // アルファブレンドを有効化
    const int blendAlpha = std::clamp(static_cast<int>(alpha_), 0, 255);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, blendAlpha);

    // 波紋の輪郭線を描画（太さを持たせるため半径を少しずらして2重描画するのも有効）
    DrawCircle(
        static_cast<int>(x_),
        static_cast<int>(y_),
        static_cast<int>(currentRadius_),
        kCircleColor,
        FALSE // 塗りつぶしなし（枠線のみ）
    );

    // ブレンドモードを通常描画に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}