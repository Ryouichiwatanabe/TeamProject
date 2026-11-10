#include "EffectRespawn.h"
#include "Player.h"
#include "PlayerPiece.h"
#include "Transform.h"
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "GameConfig.h"
#include <cmath>
#include <random>

EffectRespawn::EffectRespawn(Player* pPlayer)
    : EffectBase(0.0f, 0.0f, LIFE_TIME)
    , m_pPlayer(pPlayer)
{
    if (m_pPlayer == nullptr)
    {
        m_bIsDead = true;
        return;
    }

    // 下から上へキラキラ立ち上る復活パーティクルを初期化
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> distX(-130.0f, 130.0f);
    std::uniform_real_distribution<float> distY(-60.0f, 110.0f);
    std::uniform_real_distribution<float> distSpeed(90.0f, 170.0f);
    std::uniform_real_distribution<float> distSize(6.0f, 13.0f);
    std::uniform_real_distribution<float> distPhase(0.0f, 6.283185f);

    m_sparkles.resize(SPARKLE_COUNT);
    for (auto& sp : m_sparkles)
    {
        sp.fOffsetX = distX(gen);
        sp.fOffsetY = distY(gen);
        sp.fSpeedY = distSpeed(gen);
        sp.fSize = distSize(gen);
        sp.fPhase = distPhase(gen);
    }
}

void EffectRespawn::OnUpdate(float fDeltaTime)
{
    for (auto& sp : m_sparkles)
    {
        sp.fOffsetY -= sp.fSpeedY * fDeltaTime;
    }
}

void EffectRespawn::Draw() const
{
    if (m_bIsDead || m_pPlayer == nullptr)
        return;

    const VECTOR center = m_pPlayer->CalculateCenterOfMass();

    // 1. 復活した瞬間の「光の柱＆衝撃リング」
    DrawLightPillar(center.x, center.y);

    // 2. 各PlayerPieceの実際の座標・角度に1pxもズレずに重なるチカチカ発光
    DrawPieceBlink();

    // 3. 周りをキラキラ昇っていく復活の光の粒
    DrawRisingSparkles(center.x, center.y);

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

// 1. 上空から降り注ぐ「復活の光の柱」と広がるリング
void EffectRespawn::DrawLightPillar(float centerX, float centerY) const
{
    if (m_fElapsedTime >= PILLAR_DURATION)
        return;

    const float rate = m_fElapsedTime / PILLAR_DURATION; // 0.0 -> 1.0
    const float invRate = 1.0f - rate;

    const unsigned int pillarOuterCol = GetColor(255, 235, 160); // 温かいゴールド光
    const unsigned int pillarCoreCol = GetColor(255, 255, 245); // 芯のホワイト光

    const int cx = static_cast<int>(centerX);
    const int cy = static_cast<int>(centerY);

    // スーッと細くなりながら消える光の柱
    const int outerHalfW = static_cast<int>(130.0f * invRate);
    const int coreHalfW = static_cast<int>(65.0f * invRate);
    const int alpha = static_cast<int>(160.0f * invRate);

    SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
    DrawBox(cx - outerHalfW, 0, cx + outerHalfW, cy + 120, pillarOuterCol, TRUE);
    DrawBox(cx - coreHalfW, 0, cx + coreHalfW, cy + 120, pillarCoreCol, TRUE);

    // 同時に外へ広がる復活の輪
    const int ringR = static_cast<int>(40.0f + 150.0f * rate);
    DrawOval(cx, cy, ringR, static_cast<int>(ringR * 0.6f), pillarCoreCol, FALSE);
    DrawOval(cx, cy, ringR - 6, static_cast<int>((ringR - 6) * 0.6f), pillarOuterCol, FALSE);
}

// 2. 各PlayerPieceの実際のTransform座標・角度を直接使ってズレ0でチカチカさせる
void EffectRespawn::DrawPieceBlink() const
{
    auto scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
    auto objectManager = scene ? scene->GetObjectManager() : nullptr;
    if (!objectManager)
        return;

    const int blinkPhase = static_cast<int>(m_fElapsedTime / BLINK_INTERVAL);
    const float fadeOut = 1.0f - GetProgress();

    // 偶数フェーズは強い白フラッシュ、奇数フェーズは細い光枠だけにして「チカチカ感」を出す
    const bool isBrightPhase = (blinkPhase % 2 == 0);
    const int alpha = static_cast<int>((isBrightPhase ? 175.0f : 55.0f) * fadeOut);
    if (alpha <= 5)
        return;

    const unsigned int flashCol = GetColor(255, 252, 235);
    const unsigned int rimCol = GetColor(255, 225, 130);
    const float halfCell = GameConfig::CELL_SIZE * 0.5f;

    SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

    // シーン内の実際のPlayerPieceを探し、その正確な座標・回転角にピッタリ重ねる
    for (auto* obj : objectManager->GetObjects())
    {
        if (!obj || obj->GetTag() != Tag::PLAYER_CELL)
            continue;

        auto* piece = static_cast<PlayerPiece*>(obj);
        if (!piece->IsActive() || piece->GetPlayer() != m_pPlayer)
            continue;

        auto* pieceTransform = piece->GetModule<Transform>();
        if (!pieceTransform)
            continue;

        const VECTOR pos = pieceTransform->GetPosition();
        const float rotZ = pieceTransform->GetRotation().z;

        if (isBrightPhase)
        {
            // ピース全体にピッタリ重なる白フラッシュ
            DrawRotatedBox(pos.x, pos.y, halfCell, rotZ, flashCol, true);
        }
        // ピースのフチを囲む光の輪郭線
        DrawRotatedBox(pos.x, pos.y, halfCell + 4.0f, rotZ, rimCol, false);
    }
}

// 3. 豆腐の周りからキラキラ立ち上るダイヤ型の光の粒
void EffectRespawn::DrawRisingSparkles(float centerX, float centerY) const
{
    const float fadeOut = 1.0f - GetProgress();
    const unsigned int sparkleCol = GetColor(255, 248, 195);

    for (const auto& sp : m_sparkles)
    {
        // キラキラ明滅させる
        const float twinkle = 0.5f + 0.5f * std::sinf(m_fElapsedTime * 18.0f + sp.fPhase);
        const int alpha = static_cast<int>(210.0f * fadeOut * twinkle);
        if (alpha <= 5)
            continue;

        const int sx = static_cast<int>(centerX + sp.fOffsetX);
        const int sy = static_cast<int>(centerY + sp.fOffsetY);
        const int r = static_cast<int>(sp.fSize * (0.6f + 0.4f * twinkle));

        SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

        // ひし形（ダイヤ型）のキラキラ星を描く
        DrawQuadrangle(
            sx, sy - r,
            sx + r, sy,
            sx, sy + r,
            sx - r, sy,
            sparkleCol, TRUE
        );
    }
}

// 回転角度(rotZ)に合わせて四角形を1pxのズレもなく描くヘルパー
void EffectRespawn::DrawRotatedBox(float cx, float cy, float halfSize, float rotZ, unsigned int color, bool bFill)
{
    const float c = std::cosf(rotZ);
    const float s = std::sinf(rotZ);

    const float localCorners[4][2] = {
        { -halfSize, -halfSize },
        {  halfSize, -halfSize },
        {  halfSize,  halfSize },
        { -halfSize,  halfSize }
    };

    int wx[4]{};
    int wy[4]{};
    for (int i = 0; i < 4; ++i)
    {
        wx[i] = static_cast<int>(cx + localCorners[i][0] * c - localCorners[i][1] * s);
        wy[i] = static_cast<int>(cy + localCorners[i][0] * s + localCorners[i][1] * c);
    }

    DrawQuadrangle(
        wx[0], wy[0],
        wx[1], wy[1],
        wx[2], wy[2],
        wx[3], wy[3],
        color,
        bFill ? TRUE : FALSE
    );
}