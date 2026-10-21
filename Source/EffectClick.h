#pragma once
#include "EffectBase.h"

class EffectClick final : public EffectBase
{
public:
    EffectClick(float x, float y);
    ~EffectClick() override = default;

    void Update(float deltaTime) override;
    void Draw() const override;
    [[nodiscard]] bool IsDead() const override { return isDead_; }

private:
    float x_{ 0.0f };
    float y_{ 0.0f };
    float currentRadius_{ 0.0f };
    float alpha_{ 255.0f };
    bool isDead_{ false };
};