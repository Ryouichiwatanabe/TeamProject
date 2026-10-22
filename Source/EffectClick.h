#pragma once
#include "EffectBase.h"

class EffectClick final : public EffectBase
{
public:
    // 引数に float 型を表す 'f' プレフィックスを適用
    EffectClick(float fX, float fY);
    ~EffectClick() override = default;

    // deltaTime も float 型のため 'f' を付与
    void Update(float fDeltaTime) override;
    void Draw() const override;

    // 戻り値 bool のゲッター、メンバ変数の参照を対応
    [[nodiscard]] bool IsDead() const override { return m_bIsDead; }

private:
    // メンバ変数を表す 'm_' ＋ 各型のプレフィックス
    float m_fX = 0.0f;
    float m_fY = 0.0f;
    float m_fCurrentRadius = 0.0f;
    float m_fAlpha = 255.0f;
    bool  m_bIsDead = false;
};