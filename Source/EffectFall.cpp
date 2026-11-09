#include "EffectFall.h"
#include "GameConfig.h"
#include <random>
#include <numbers>
#include <cmath>

EffectFall::EffectFall(float fX, float fY, float fOutwardAngleRad)
    : EffectBase(fX, fY, EFFECT_LIFETIME)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // 辺に沿った方向（外向きベクトルと直角の方向）に発生位置をバラけさせる
    const float halfEdge = GameConfig::CELL_SIZE * 0.42f;
    std::uniform_real_distribution<float> distAlongEdge(-halfEdge, halfEdge);

    // 外向き角度から ±40度（約0.7ラジアン）の範囲で外側へ扇状に飛び散らせる
    std::uniform_real_distribution<float> distAngleOffset(-0.7f, 0.7f);

    // 飛び出すスピード (px/s)
    std::uniform_real_distribution<float> distSpeed(130.0f, 280.0f);

    // ドットのサイズ（2px単位でカクッとしたドット感を出す：4, 6, 8px）
    std::uniform_int_distribution<int> distSizeStep(2, 4);

    // 辺の方向ベクトル（外向き角度 + 90度）
    const float edgeDirX = -std::sin(fOutwardAngleRad);
    const float edgeDirY = std::cos(fOutwardAngleRad);

    m_drops.reserve(DROP_COUNT);

    for (int i = 0; i < DROP_COUNT; ++i)
    {
        const float edgeOffset = distAlongEdge(gen);
        const float flyAngle = fOutwardAngleRad + distAngleOffset(gen);
        const float speed = distSpeed(gen);

        SplashDrop drop;
        // ブロックのフチ（辺の上）からスタート
        drop.fX = m_fX + edgeDirX * edgeOffset;
        drop.fY = m_fY + edgeDirY * edgeOffset;
        drop.fVelocityX = std::cos(flyAngle) * speed;
        drop.fVelocityY = std::sin(flyAngle) * speed - 60.0f; // 少しだけ上向きの勢いを足す
        drop.nPixelSize = distSizeStep(gen) * 2;              // 4px, 6px, 8px のドット
        drop.bIsWhite = (i % 3 == 0);

        m_drops.emplace_back(drop);
    }
}

void EffectFall::OnUpdate(float fDeltaTime)
{
    for (auto& drop : m_drops)
    {
        drop.fX += drop.fVelocityX * fDeltaTime;
        drop.fY += drop.fVelocityY * fDeltaTime;
        drop.fVelocityY += GRAVITY * fDeltaTime;
        drop.fVelocityX *= 0.96f;
    }
}

void EffectFall::Draw() const
{
    const float fProgress = GetProgress();

    // ドット絵らしく、消えるときは段階的にサイズを小さくしていく
    const float fScale = 1.0f - (fProgress * 0.75f);
    if (fScale <= 0.1f) return;

    // ドット絵に映えるパキッとした水色・濃い水色・白
    const unsigned int colorMain = GetColor(110, 210, 255); // 鮮やかな水色
    const unsigned int colorWhite = GetColor(245, 252, 255); // ハイライトの白
    const unsigned int colorShadow = GetColor(50, 140, 210);  // ドットのフチ・影色

    // 後半だけ少し透過させる
    const int nAlpha = (fProgress < 0.6f) ? 255 : static_cast<int>(255.0f * (1.0f - (fProgress - 0.6f) / 0.4f));
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, nAlpha);

    for (const auto& drop : m_drops)
    {
        // 座標を 2px グリッドにスナップさせて「ドット絵の動き」にする
        const int snapX = (static_cast<int>(drop.fX) / 2) * 2;
        const int snapY = (static_cast<int>(drop.fY) / 2) * 2;

        // サイズも 2px 単位にする
        int size = static_cast<int>(drop.nPixelSize * fScale);
        size = (size / 2) * 2;
        if (size < 2) size = 2;

        const int half = size / 2;
        const unsigned int fillCol = drop.bIsWhite ? colorWhite : colorMain;

        // 1. 1ドット下に濃い水色の影ドットを描いて立体的なピクセルアート感を出す
        DrawBox(
            snapX - half,
            snapY - half + 2,
            snapX + half,
            snapY + half + 2,
            colorShadow,
            TRUE
        );

        // 2. メインの四角いドット（水飛沫）を描画
        DrawBox(
            snapX - half,
            snapY - half,
            snapX + half,
            snapY + half,
            fillCol,
            TRUE
        );

        // 3. 大きめの粒には左上に2×2pxの白いハイライトドットを打つ
        if (size >= 6 && !drop.bIsWhite)
        {
            DrawBox(
                snapX - half,
                snapY - half,
                snapX - half + 2,
                snapY - half + 2,
                colorWhite,
                TRUE
            );
        }
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}