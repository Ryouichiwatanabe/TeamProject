#pragma once

class EffectBase
{
public:
    virtual ~EffectBase() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() const = 0;
    [[nodiscard]] virtual bool IsDead() const = 0;
};