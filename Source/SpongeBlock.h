#pragma once
#include "StageBlock.h"

class Player;

// 乗るとボヨンと跳ねて豆腐を高くジャンプさせる「キッチンスポンジ」ブロック
class SpongeBlock : public StageBlock
{
public:
    ~SpongeBlock() override = default;

    void Init() override;
    void Update(float _deltaTime) override;
    void Draw() override;

private:
    // GetModule を呼べるように末尾の const を外しました
    Player* FindPlayerOnTop();
    void    UpdateJumpCarry(float _deltaTime);
    bool    HasCeilingAbovePlayer(Player* pPlayer, float nextStepY);

private:
    static constexpr float INITIAL_JUMP_SPEED = 15000.0f;  // 跳ね上げる初速(px/秒)
    static constexpr float JUMP_DECEL = 2100.0f; // 上昇中の減速度(px/秒^2)
    static constexpr float BOUNCE_ANIM_TIME = 0.45f;   // スポンジがボヨンと揺れる秒数
    static constexpr float COOLDOWN_TIME = 0.35f;   // 連続発火を防ぐクールタイム

    Player* mpJumpingPlayer = nullptr; // 現在ジャンプさせているプレイヤー
    float   mfJumpSpeedY = 0.0f;    // 現在の上昇速度
    float   mfBounceTimer = 0.0f;    // スポンジ伸縮アニメ用タイマー
    float   mfCooldown = 0.0f;    // 再発火クールタイム
};