#pragma once
#include "StageBlock.h"
#include "EffectSmokeSpriteSheet.h"
#include <memory>

class Spike : public StageBlock
{
public:
    ~Spike() override = default;

    void Init() override;
    void InitComponent() override;

    void Update(float _deltaTime) override;
    void Draw() override;

private:
    void CheckPlayerCollision();

private:
    // 既存のエフェクトスモークスプライトシートを用いてアニメーション描画
    std::unique_ptr<EffectSmokeSpriteSheet> mpSmokeEffect = nullptr;
};