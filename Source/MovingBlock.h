#pragma once
#include "StageBlock.h"
#include <DxLib.h>

class Player;

// プレイヤーを乗せて往復移動する床ブロック
class MovingBlock : public StageBlock
{
public:
    ~MovingBlock() override = default;

    void Init() override;
    void Update(float _deltaTime) override;
    void Draw() override;

protected:
    // 画像リソース（パスを変えると見た目を変更可能）
    const char* GetModelFilename() const override
    {
        return "Resource/GameObject/StageBlock/Stage.png";
    }

private:
    Player* FindPlayerOnTop(const VECTOR& blockPos); // 上に乗っているプレイヤーの取得

private:
    // 移動設定
    static constexpr float MOVE_RANGE = 200.0f;       // 移動幅（片道のpx）
    static constexpr float MOVE_SPEED = 2.0f;         // 移動速度
    static constexpr bool  IS_VERTICAL_MOVE = false;  // trueで上下移動、falseで左右移動

    VECTOR mvStartPos = VGet(0.0f, 0.0f, 0.0f); // 初期位置
    bool   mbStartPosSaved = false;
    float  mfMoveTimer = 0.0f;
};