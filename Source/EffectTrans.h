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
    // 9×9 の整ったドット絵スターを描画
    static void DrawPixelStar(int cx, int cy, int dotSize, int pattern, unsigned int mainCol, unsigned int edgeCol, unsigned int hiCol);
    // 5×5 のミニ豆腐キューブ（かけら）を描画
    static void DrawMiniTofu(int cx, int cy, int dotSize, unsigned int mainCol, unsigned int edgeCol, unsigned int hiCol);

private:
    struct TofuStarParticle
    {
        float fX = 0.0f;
        float fY = 0.0f;
        float fVelocityX = 0.0f;
        float fVelocityY = 0.0f;
        int   nDotSize = 3;       // 1ドットの大きさ（3px?4pxのちょうどいいドット感）
        int   nColorType = 0;     // 0:豆腐白, 1:大豆クリーム, 2:湧き水ブルー, 3:薬味グリーン
        int   nAnimOffset = 0;
        bool  bIsMiniTofu = false;// false:ドット星, true:ミニ豆腐キューブ
    };

    std::vector<TofuStarParticle> m_particles;

    static constexpr float EFFECT_LIFETIME = 0.48f;
    static constexpr int   PARTICLE_COUNT = 6;
};