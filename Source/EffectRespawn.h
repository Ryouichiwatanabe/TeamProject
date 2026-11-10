#pragma once
#include "EffectBase.h"

class Player;

// リスポーン時にプレイヤーのブロック自体をシンプルにチカチカ点滅させるエフェクト
class EffectRespawn final : public EffectBase
{
public:
    // --- 調整用定数 ---
    static constexpr float LIFE_TIME = 1.0f;  // 点滅する合計時間（秒）
    static constexpr float BLINK_INTERVAL = 0.08f; // チカチカ切り替わる速さ（秒）

public:
    explicit EffectRespawn(Player* pPlayer);
    ~EffectRespawn() override;

    void OnUpdate(float fDeltaTime) override;
    void Draw() const override;

private:
    // プレイヤーの全ブロックの表示(true) / 非表示(false)を切り替える
    void SetPlayerBlocksVisible(bool bVisible) const;

private:
    Player* m_pPlayer = nullptr;
};