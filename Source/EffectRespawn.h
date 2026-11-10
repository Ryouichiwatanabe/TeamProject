#pragma once
#include "EffectBase.h"
#include <vector>

class Player;

// プレイヤーがリスポーンした時の「光の柱＋ピッタリ追従チカチカ＋上昇キラキラ」エフェクト
class EffectRespawn final : public EffectBase
{
public:
    static constexpr float LIFE_TIME = 1.2f;  // 全体の再生秒数
    static constexpr float PILLAR_DURATION = 0.40f; // 最初の「光の柱」が出る秒数
    static constexpr float BLINK_INTERVAL = 0.07f; // チカチカ点滅の間隔（秒）
    static constexpr int   SPARKLE_COUNT = 14;    // 立ち上るキラキラ粒の数

public:
    explicit EffectRespawn(Player* pPlayer);
    ~EffectRespawn() override = default;

    void OnUpdate(float fDeltaTime) override;
    void Draw() const override;

private:
    struct RespawnSparkle
    {
        float fOffsetX = 0.0f; // 中心からの横ズレ
        float fOffsetY = 0.0f; // 中心からの縦ズレ
        float fSpeedY = 0.0f; // 上昇スピード
        float fSize = 0.0f; // キラキラの大きさ
        float fPhase = 0.0f; // 明滅の位相
    };

    void DrawLightPillar(float centerX, float centerY) const; // 1. 復活の光の柱＆リング
    void DrawPieceBlink() const;                              // 2. 1pxもズレないピース点滅
    void DrawRisingSparkles(float centerX, float centerY) const; // 3. 上昇する光の粒

    // 回転した四角形をズレなく描くヘルパー
    static void DrawRotatedBox(float cx, float cy, float halfSize, float rotZ, unsigned int color, bool bFill);

private:
    Player* m_pPlayer = nullptr;
    std::vector<RespawnSparkle> m_sparkles;
};