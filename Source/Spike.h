#pragma once
#include "StageBlock.h"
#include "SpriteSheet.h"
#include <memory>

// 触れるとリスポーンするお邪魔ギミック（湯気・トゲ）
class Spike : public StageBlock
{
public:
    ~Spike() override = default;

    void Init() override;
    void InitComponent() override;

    void Update(float _deltaTime) override;
    void Draw() override;

private:
    void CheckPlayerCollision(); // プレイヤーとの接触判定

private:
    std::unique_ptr<SpriteSheet> mpSmokeEffect = nullptr; // アニメーション描画用
};