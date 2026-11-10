#pragma once
#include "StageBlock.h"

class Player;

// 乗ると姿勢を保ったままツルツル横へ滑る「溶けバター」ブロック
class ButterBlock : public StageBlock
{
public:
    ~ButterBlock() override = default;

    void Init() override;
    void Update(float _deltaTime) override;
    void Draw() override;

private:
    //  GetModule を呼べるように末尾の const を外しました
    Player* FindPlayerOnTop();
    bool    CanSlideHorizontally(Player* pPlayer, float stepX);

private:
    static constexpr float SLIDE_SPEED = 10000.0f; // 滑るスピード(px/秒)

    float mfSlideDir = 1.0f; // 滑る向き（+1.0f:右 / -1.0f:左）
    float mfAnimTimer = 0.0f; // ツヤ・矢印アニメーション用タイマー
};