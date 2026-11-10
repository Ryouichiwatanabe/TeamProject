#include "SpongeBlock.h"
#include "Player.h"
#include "PlayerPiece.h"

// -- Module -- //
#include "Transform.h"
#include "Collider.h"

// -- Manager -- //
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "GameConfig.h"

#include <DxLib.h>
#include <cmath>
#include <algorithm>

void SpongeBlock::Init()
{
    StageBlock::Init();
}

void SpongeBlock::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
        return;

    if (mfBounceTimer > 0.0f)
        mfBounceTimer -= _deltaTime;

    if (mfCooldown > 0.0f)
        mfCooldown -= _deltaTime;

    // プレイヤーが上に乗ったらジャンプ発火
    if (mfCooldown <= 0.0f)
    {
        if (Player* pPlayer = FindPlayerOnTop())
        {
            mpJumpingPlayer = pPlayer;
            mfJumpSpeedY = INITIAL_JUMP_SPEED;
            mfBounceTimer = BOUNCE_ANIM_TIME;
            mfCooldown = COOLDOWN_TIME;
        }
    }

    UpdateJumpCarry(_deltaTime);

    StageBlock::Update(_deltaTime);
}

// スポンジの真上に乗っているプレイヤーを検出（★末尾の const を削除済み）
Player* SpongeBlock::FindPlayerOnTop()
{
    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return nullptr;

    auto* scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
    auto* objectManager = scene ? scene->GetObjectManager() : nullptr;
    if (!objectManager)
        return nullptr;

    const VECTOR myPos = myTransform->GetPosition();
    const float halfCell = GameConfig::CELL_SIZE * 0.5f;
    const float spongeTop = myPos.y - halfCell;

    for (auto* obj : objectManager->GetObjects())
    {
        if (!obj || obj->GetTag() != Tag::PLAYER_CELL)
            continue;

        auto* piece = static_cast<PlayerPiece*>(obj);
        if (!piece->IsActive())
            continue;

        auto* pieceTransform = piece->GetModule<Transform>();
        if (!pieceTransform)
            continue;

        const VECTOR piecePos = pieceTransform->GetPosition();
        const float pieceBottom = piecePos.y + halfCell;

        const bool isWithinX = std::fabsf(piecePos.x - myPos.x) < (GameConfig::CELL_SIZE * 0.85f);
        const bool isTouchingTop = (pieceBottom >= spongeTop - 12.0f) && (pieceBottom <= spongeTop + 28.0f);

        if (isWithinX && isTouchingTop)
        {
            return piece->GetPlayer();
        }
    }

    return nullptr;
}

// ジャンプ中のプレイヤーを滑らかに上空へ運ぶ
void SpongeBlock::UpdateJumpCarry(float _deltaTime)
{
    if (!mpJumpingPlayer || mfJumpSpeedY <= 0.0f)
        return;

    auto* playerTransform = mpJumpingPlayer->GetModule<Transform>();
    if (!playerTransform)
    {
        mpJumpingPlayer = nullptr;
        return;
    }

    const float stepY = -mfJumpSpeedY * _deltaTime;

    if (HasCeilingAbovePlayer(mpJumpingPlayer, stepY))
    {
        mfJumpSpeedY = 0.0f;
        mpJumpingPlayer = nullptr;
        return;
    }

    VECTOR pos = playerTransform->GetPosition();
    pos.y += stepY;
    playerTransform->SetPosition(pos);
    mpJumpingPlayer->UpdatePieceTransforms();

    mfJumpSpeedY -= JUMP_DECEL * _deltaTime;
    if (mfJumpSpeedY <= 0.0f)
    {
        mfJumpSpeedY = 0.0f;
        mpJumpingPlayer = nullptr;
    }
}

// ジャンプ中に天井を突き抜けないためのチェック（★末尾の const を削除済み）
bool SpongeBlock::HasCeilingAbovePlayer(Player* pPlayer, float nextStepY)
{
    auto* scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
    auto* objectManager = scene ? scene->GetObjectManager() : nullptr;
    if (!objectManager)
        return false;

    const float halfCell = GameConfig::CELL_SIZE * 0.5f;

    for (auto* obj : objectManager->GetObjects())
    {
        if (!obj || obj->GetTag() != Tag::PLAYER_CELL)
            continue;

        auto* piece = static_cast<PlayerPiece*>(obj);
        if (!piece->IsActive() || piece->GetPlayer() != pPlayer)
            continue;

        const VECTOR piecePos = piece->GetModule<Transform>()->GetPosition();
        const float nextTop = piecePos.y + nextStepY - halfCell;

        for (auto* blockObj : objectManager->GetObjects())
        {
            if (!blockObj || blockObj == this || blockObj->GetTag() != Tag::BLOCK)
                continue;

            auto* blockCol = blockObj->GetModule<Collider>();
            if (!blockCol || !blockCol->IsEnabled())
                continue;

            const VECTOR blockPos = blockObj->GetModule<Transform>()->GetPosition();
            if (std::fabsf(piecePos.x - blockPos.x) < GameConfig::CELL_SIZE * 0.85f)
            {
                const float blockBottom = blockPos.y + halfCell;
                if (piecePos.y >= blockPos.y && nextTop <= blockBottom)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// 2層キッチンスポンジの描画
void SpongeBlock::Draw()
{
    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return;

    const VECTOR pos = myTransform->GetPosition();
    const float half = GameConfig::CELL_SIZE * 0.5f;

    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (mfBounceTimer > 0.0f)
    {
        const float t = BOUNCE_ANIM_TIME - mfBounceTimer;
        const float wave = std::sinf(t * 28.0f) * std::expf(-t * 6.0f);
        scaleY = 1.0f + wave * 0.38f;
        scaleX = 1.0f - wave * 0.22f;
    }

    const int cx = static_cast<int>(pos.x);
    const int bottom = static_cast<int>(pos.y + half);
    const int width = static_cast<int>(GameConfig::CELL_SIZE * scaleX);
    const int height = static_cast<int>(GameConfig::CELL_SIZE * scaleY);
    const int left = cx - width / 2;
    const int right = cx + width / 2;
    const int top = bottom - height;
    const int greenH = height / 4;

    const unsigned int shadowCol = GetColor(20, 18, 15);
    const unsigned int yellowCol = GetColor(255, 220, 65);
    const unsigned int holeCol = GetColor(225, 178, 35);
    const unsigned int greenCol = GetColor(55, 168, 82);
    const unsigned int rimCol = GetColor(125, 95, 30);
    const unsigned int bubbleCol = GetColor(245, 252, 255);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 75);
    DrawBox(left + 6, top + 8, right + 6, bottom + 8, shadowCol, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawBox(left, top + greenH, right, bottom, yellowCol, TRUE);

    DrawCircle(cx - width / 4, top + greenH + height / 4, 6, holeCol, TRUE);
    DrawCircle(cx + width / 5, bottom - height / 4, 7, holeCol, TRUE);
    DrawCircle(cx + width / 4, top + greenH + height / 5, 4, holeCol, TRUE);
    DrawCircle(cx - width / 5, bottom - height / 5, 5, holeCol, TRUE);

    DrawBox(left - 2, top, right + 2, top + greenH, greenCol, TRUE);
    DrawBox(left, top, right, bottom, rimCol, FALSE);

    if (mfBounceTimer > 0.0f)
    {
        const float rate = 1.0f - (mfBounceTimer / BOUNCE_ANIM_TIME);
        const int alpha = static_cast<int>(200.0f * (1.0f - rate));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        for (int i = -2; i <= 2; ++i)
        {
            const int bx = cx + i * 24;
            const int by = top - static_cast<int>(rate * 55.0f) + (i * i * 4);
            DrawCircle(bx, by, 7, bubbleCol, FALSE);
            DrawCircle(bx - 2, by - 2, 2, bubbleCol, TRUE);
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}