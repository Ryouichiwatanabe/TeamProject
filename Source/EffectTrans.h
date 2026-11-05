#pragma once
#include "EffectBase.h"
#include <vector>

class EffectTrans final : public EffectBase
{
public:
    EffectTrans(float fX, float fY, float fOutwardAngleRad = 0.0f);
    ~EffectTrans() override = default;

    void Draw() const override;

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    // 存在感のある特大の立体豆腐ブロック片（四角）を描画
    static void DrawBigTofuBlock(int cx, int cy, int halfSize, int colorType);

private:
    struct BigTofuChunk
    {
        float fX = 0.0f;
        float fY = 0.0f;
        float fVelocityX = 0.0f;
        float fVelocityY = 0.0f;
        float fBaseHalfSize = 18.0f; // 半径16?26px（直径32?52pxのデカい四角！）
        int   nColorType = 0;        // 0:絹ごし白, 1:木綿アイボリー, 2:大豆クリーム
    };

    std::vector<BigTofuChunk> m_chunks;

    static constexpr float EFFECT_LIFETIME = 0.45f;
    static constexpr int   CHUNK_COUNT = 1;     // 細かい粒は出さず、デカい塊を3個ドカンと飛ばす！
};