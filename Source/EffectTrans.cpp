#include "EffectTrans.h"
#include "GameConfig.h"
#include <random>
#include <cmath>

EffectTrans::EffectTrans(float fX, float fY, float fOutwardAngleRad)
    : EffectBase(fX, fY, EFFECT_LIFETIME)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    const float halfEdge = GameConfig::CELL_SIZE * 0.38f;
    std::uniform_real_distribution<float> distAlongEdge(-halfEdge, halfEdge);

    // 外向きに豪快に広がる角度
    std::uniform_real_distribution<float> distAngleOffset(-0.75f, 0.75f);

    // ドカン！と力強く吹き飛ぶ初速 (350?650 px/s)
    std::uniform_real_distribution<float> distSpeed(350.0f, 650.0f);

    // ★細かい粒は一切なし！ 半径16px?26px（全体で32px?52px）の特大サイズのみ！
    std::uniform_real_distribution<float> distHalfSize(16.0f, 26.0f);

    const float edgeDirX = -std::sin(fOutwardAngleRad);
    const float edgeDirY = std::cos(fOutwardAngleRad);

    m_chunks.reserve(CHUNK_COUNT);

    for (int i = 0; i < CHUNK_COUNT; ++i)
    {
        const float edgeOffset = distAlongEdge(gen);
        const float flyAngle = fOutwardAngleRad + distAngleOffset(gen);
        const float speed = distSpeed(gen);

        BigTofuChunk chunk;
        chunk.fX = m_fX + edgeDirX * edgeOffset;
        chunk.fY = m_fY + edgeDirY * edgeOffset;
        chunk.fVelocityX = std::cos(flyAngle) * speed;
        chunk.fVelocityY = std::sin(flyAngle) * speed - 50.0f;
        chunk.fBaseHalfSize = distHalfSize(gen);
        chunk.nColorType = i % 3;

        m_chunks.emplace_back(chunk);
    }
}

void EffectTrans::OnUpdate(float fDeltaTime)
{
    for (auto& chunk : m_chunks)
    {
        chunk.fX += chunk.fVelocityX * fDeltaTime;
        chunk.fY += chunk.fVelocityY * fDeltaTime;

        // ドカンと飛び出してキュッと止まる力強い減速
        const float drag = std::pow(0.83f, fDeltaTime * 60.0f);
        chunk.fVelocityX *= drag;
        chunk.fVelocityY *= drag;

        // 重力でゴロンと落ちる
        chunk.fVelocityY += 450.0f * fDeltaTime;
    }
}

// 太い輪郭と立体的な影がついた「特大の豆腐ブロック（四角）」を描く関数
void EffectTrans::DrawBigTofuBlock(int cx, int cy, int halfSize, int colorType)
{
    if (halfSize < 4) return;

    // 豆腐の3色バリエーション（すべて落ち着いた豆腐色）
    const unsigned int mainColors[3] = {
        GetColor(248, 244, 230), // 絹ごし豆腐色
        GetColor(240, 232, 210), // 木綿豆腐色
        GetColor(245, 228, 190)  // 大豆クリーム色
    };
    const unsigned int shadowColors[3] = {
        GetColor(222, 214, 195), // 立体感を出す下側の影色
        GetColor(212, 200, 175),
        GetColor(218, 198, 155)
    };
    const unsigned int outlineCol = GetColor(55, 45, 38); // パキッと目立たせる太いこげ茶の輪郭
    const unsigned int hiCol = GetColor(255, 255, 250); // 上面のハイライト

    const int x1 = cx - halfSize;
    const int y1 = cy - halfSize;
    const int x2 = cx + halfSize;
    const int y2 = cy + halfSize;
    const int border = 3; // 3pxの太い輪郭線でデカさと迫力を強調！

    // 1. 一番外側の太い輪郭（こげ茶・黒系）
    DrawBox(x1 - border, y1 - border, x2 + border, y2 + border, outlineCol, TRUE);

    // 2. 豆腐のベース色（全体）
    DrawBox(x1, y1, x2, y2, mainColors[colorType], TRUE);

    // 3. 下側・右側に厚みの影を入れて「塊（ブロック）感」を出す
    const int shadeWidth = halfSize / 2;
    DrawBox(x1, y2 - shadeWidth, x2, y2, shadowColors[colorType], TRUE);
    DrawBox(x2 - shadeWidth, y1, x2, y2, shadowColors[colorType], TRUE);

    // 4. 左上に太い白ハイライトを入れてパキッとさせる
    if (halfSize >= 10)
    {
        DrawBox(x1 + 3, y1 + 3, cx, y1 + 7, hiCol, TRUE);
        DrawBox(x1 + 3, y1 + 3, x1 + 7, cy, hiCol, TRUE);
    }
}

void EffectTrans::Draw() const
{
    const float fProgress = GetProgress();
    if (fProgress >= 1.0f) return;

    // =============================================================
    // 1. 発生瞬間の「ドカン！」という太い四角の衝撃波枠（キラキラせず力強い枠）
    // =============================================================
    if (fProgress < 0.25f)
    {
        const float rate = fProgress / 0.25f;
        const int expand = static_cast<int>(20.0f + 75.0f * rate);
        const int bx = static_cast<int>(m_fX);
        const int by = static_cast<int>(m_fY);

        const int alpha = static_cast<int>(255.0f * (1.0f - rate));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        const unsigned int waveCol = GetColor(248, 244, 230);
        const unsigned int waveOut = GetColor(70, 58, 48);

        // 太さ4pxの力強い四角い衝撃波
        for (int t = 0; t < 4; ++t)
        {
            DrawBox(bx - expand - t, by - expand - t, bx + expand + t, by + expand + t, waveCol, FALSE);
        }
        DrawBox(bx - expand - 5, by - expand - 5, bx + expand + 5, by + expand + 5, waveOut, FALSE);
    }

    // =============================================================
    // 2. 特大サイズの豆腐ブロック片（四角）をドカンと描画！
    // =============================================================
    // 最初に1.3倍にボッと大きく膨らんで迫力を出し、最後にシュッと縮んで消える
    float scale = 1.0f;
    if (fProgress < 0.18f)
    {
        scale = 0.7f + 0.6f * (fProgress / 0.18f); // 最大 1.3倍！
    }
    else
    {
        scale = 1.3f * (1.0f - (fProgress - 0.18f) / 0.82f);
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    for (const auto& chunk : m_chunks)
    {
        // 4px グリッドにスナップさせて重厚なドット感を出す
        const int snapX = (static_cast<int>(chunk.fX) / 4) * 4;
        const int snapY = (static_cast<int>(chunk.fY) / 4) * 4;

        const int currentHalfSize = static_cast<int>(chunk.fBaseHalfSize * scale);
        if (currentHalfSize < 4) continue;

        DrawBigTofuBlock(snapX, snapY, currentHalfSize, chunk.nColorType);
    }
}