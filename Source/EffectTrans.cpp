#include "EffectTrans.h"
#include "GameConfig.h"
#include <random>
#include <cmath>

EffectTrans::EffectTrans(float fX, float fY, float fOutwardAngleRad)
    : EffectBase(fX, fY, EFFECT_LIFETIME)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    const float halfEdge = GameConfig::CELL_SIZE * 0.45f;
    std::uniform_real_distribution<float> distAlongEdge(-halfEdge, halfEdge);

    // 外向きにパァン！と広がる角度
    std::uniform_real_distribution<float> distAngleOffset(-0.85f, 0.85f);

    // 気持ちよく弾け飛ぶスピード (240?480 px/s)
    std::uniform_real_distribution<float> distSpeed(240.0f, 480.0f);

    // 1ドットのサイズ（3px または 4px → 星全体で 27px?36px の上品なサイズ）
    std::uniform_int_distribution<int> distDotSize(3, 4);

    const float edgeDirX = -std::sin(fOutwardAngleRad);
    const float edgeDirY = std::cos(fOutwardAngleRad);

    m_particles.reserve(PARTICLE_COUNT);

    for (int i = 0; i < PARTICLE_COUNT; ++i)
    {
        const float edgeOffset = distAlongEdge(gen);
        const float flyAngle = fOutwardAngleRad + distAngleOffset(gen);
        const float speed = distSpeed(gen);

        TofuStarParticle p;
        p.fX = m_fX + edgeDirX * edgeOffset;
        p.fY = m_fY + edgeDirY * edgeOffset;
        p.fVelocityX = std::cos(flyAngle) * speed;
        p.fVelocityY = std::sin(flyAngle) * speed;
        p.nDotSize = distDotSize(gen);
        p.nColorType = i % 4;
        p.nAnimOffset = i;
        p.bIsMiniTofu = (i % 3 == 1); // 3個に1個は「ミニ豆腐キューブ」、残りは「ドット絵の星」

        m_particles.emplace_back(p);
    }
}

void EffectTrans::OnUpdate(float fDeltaTime)
{
    for (auto& p : m_particles)
    {
        p.fX += p.fVelocityX * fDeltaTime;
        p.fY += p.fVelocityY * fDeltaTime;

        // 最初にパァン！と広がり、空中でふわっと減速する
        const float drag = std::pow(0.86f, fDeltaTime * 60.0f);
        p.fVelocityX *= drag;
        p.fVelocityY *= drag;

        // 軽い重力でコロンと落ちる
        p.fVelocityY += 180.0f * fDeltaTime;
    }
}

// 9×9 マスの繊細でかわいいドット絵スターを描く関数
void EffectTrans::DrawPixelStar(int cx, int cy, int dotSize, int pattern, unsigned int mainCol, unsigned int edgeCol, unsigned int hiCol)
{
    // 0:透明, 1:やさしい茶フチ, 2:メイン色(豆腐・大豆・水色), 3:純白ハイライト
    static constexpr int STAR_A[9][9] = {
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 2, 1, 0, 0, 0 },
        { 0, 0, 1, 2, 3, 2, 1, 0, 0 },
        { 0, 1, 2, 3, 3, 3, 2, 1, 0 },
        { 1, 2, 3, 3, 3, 3, 3, 2, 1 },
        { 0, 1, 2, 3, 3, 3, 2, 1, 0 },
        { 0, 0, 1, 2, 3, 2, 1, 0, 0 },
        { 0, 0, 0, 1, 2, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 }
    };

    // 斜めきらめき付きの星パターン
    static constexpr int STAR_B[9][9] = {
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
        { 0, 1, 0, 1, 2, 1, 0, 1, 0 },
        { 0, 0, 2, 2, 3, 2, 2, 0, 0 },
        { 0, 1, 2, 3, 3, 3, 2, 1, 0 },
        { 1, 2, 3, 3, 3, 3, 3, 2, 1 },
        { 0, 1, 2, 3, 3, 3, 2, 1, 0 },
        { 0, 0, 2, 2, 3, 2, 2, 0, 0 },
        { 0, 1, 0, 1, 2, 1, 0, 1, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 0 }
    };

    const auto& grid = (pattern % 2 == 0) ? STAR_A : STAR_B;
    const int startX = cx - (9 * dotSize) / 2;
    const int startY = cy - (9 * dotSize) / 2;

    for (int r = 0; r < 9; ++r)
    {
        for (int c = 0; c < 9; ++c)
        {
            const int cell = grid[r][c];
            if (cell == 0) continue;

            const int x1 = startX + c * dotSize;
            const int y1 = startY + r * dotSize;

            unsigned int col = mainCol;
            if (cell == 1) col = edgeCol;
            else if (cell == 3) col = hiCol;

            DrawBox(x1, y1, x1 + dotSize, y1 + dotSize, col, TRUE);
        }
    }
}

// 5×5 マスの「ミニ豆腐ブロック」を描く関数
void EffectTrans::DrawMiniTofu(int cx, int cy, int dotSize, unsigned int mainCol, unsigned int edgeCol, unsigned int hiCol)
{
    static constexpr int TOFU_MAP[5][5] = {
        { 0, 1, 1, 1, 0 },
        { 1, 3, 3, 2, 1 },
        { 1, 3, 2, 2, 1 },
        { 1, 2, 2, 2, 1 },
        { 0, 1, 1, 1, 0 }
    };

    const int d = dotSize + 1; // 豆腐キューブは少しぷっくりさせる
    const int startX = cx - (5 * d) / 2;
    const int startY = cy - (5 * d) / 2;

    for (int r = 0; r < 5; ++r)
    {
        for (int c = 0; c < 5; ++c)
        {
            const int cell = TOFU_MAP[r][c];
            if (cell == 0) continue;

            const int x1 = startX + c * d;
            const int y1 = startY + r * d;

            unsigned int col = mainCol;
            if (cell == 1) col = edgeCol;
            else if (cell == 3) col = hiCol;

            DrawBox(x1, y1, x1 + d, y1 + d, col, TRUE);
        }
    }
}

void EffectTrans::Draw() const
{
    const float fProgress = GetProgress();
    if (fProgress >= 1.0f) return;

    // 豆腐コンセプトに合わせた和風・やさしいカラーパレット
    const unsigned int mainPalette[4] = {
        GetColor(250, 246, 232), // ① 木綿とうふホワイト（クリーム白）
        GetColor(248, 228, 165), // ② 大豆・湯葉イエロー（やさしい黄色）
        GetColor(180, 232, 250), // ③ 湧き水アクア（澄んだ水色）
        GetColor(175, 230, 160)  // ④ 薬味ネギグリーン（爽やかな若草色）
    };
    const unsigned int edgeCol = GetColor(95, 80, 68); // 豆腐の輪郭に馴染むやさしい茶色フチ
    const unsigned int hiCol = GetColor(255, 255, 255); // ツヤっとした純白ハイライト

    // 後半にかけて自然にフェードアウト＆ドット縮小
    const int nAlpha = (fProgress < 0.6f) ? 255 : static_cast<int>(255.0f * (1.0f - (fProgress - 0.6f) / 0.4f));
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, nAlpha);

    const int animFrame = static_cast<int>(m_fElapsedTime / 0.08f);

    for (const auto& p : m_particles)
    {
        // 2px グリッドにスナップ（荒すぎず、ドット絵らしい動きに）
        const int snapX = (static_cast<int>(p.fX) / 2) * 2;
        const int snapY = (static_cast<int>(p.fY) / 2) * 2;

        // 消え際に1段階小さくなる
        int dotSize = (fProgress < 0.7f) ? p.nDotSize : (p.nDotSize - 1);
        if (dotSize < 2) dotSize = 2;

        if (p.bIsMiniTofu)
        {
            // コロンとしたミニ豆腐キューブを描画
            DrawMiniTofu(snapX, snapY, dotSize, mainPalette[0], edgeCol, hiCol);
        }
        else
        {
            // 9×9 の整ったドット絵スターを描画
            DrawPixelStar(
                snapX,
                snapY,
                dotSize,
                animFrame + p.nAnimOffset,
                mainPalette[p.nColorType],
                edgeCol,
                hiCol
            );
        }
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}