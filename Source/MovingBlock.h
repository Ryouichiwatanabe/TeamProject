#pragma once
#include "StageBlock.h"
#include <DxLib.h>

class Player;

// CSVで置いた位置を基準に往復移動し、上に乗った豆腐も一緒に運ぶ「動く床（まな板リフト）」
class MovingBlock : public StageBlock
{
public:
    ~MovingBlock() override = default;

    void Init() override;
    void Update(float _deltaTime) override;
    void Draw() override;

private:
    Player* FindPlayerOnTop(const VECTOR& blockPos); // この床の上に乗っているプレイヤーを探す

private:
    // =========================================================================
    // ★ 動く床の「移動幅」「スピード」「動く方向」を調整する定数
    // =========================================================================

    // [片道の移動距離(px)]
    // ・初期値 200.0f（2マス分）→ 置いた位置から左右に ±200px（合計4マス幅）を往復する
    // ・大きくする(例: 300.0f) → より広い範囲を大きく往復するようになる
    static constexpr float MOVE_RANGE = 250.0f;

    // [移動スピード]
    // ・大きくする(例: 3.5f) → 往復が速くなる
    // ・小さくする(例: 1.0f) → ゆっくり動くようになる
    static constexpr float MOVE_SPEED = 1.0f;

    // [動く方向の切り替え]
    // ・false（初期値）→ 「左右（横）」に往復する動く床
    // ・true に変更する → 「上下（縦）」に昇降するエレベーター床になる！
    static constexpr bool  IS_VERTICAL_MOVE = false;

    VECTOR mvStartPos = VGet(0.0f, 0.0f, 0.0f); // CSVで配置された初期座標
    bool   mbStartPosSaved = false;                  // 初期座標を記憶したか
    float  mfMoveTimer = 0.0f;                   // 往復移動用タイマー
};