#include "ButterBlock.h"
#include "Player.h"
#include "PlayerPiece.h"

// -- Module -- //
#include "Transform.h"
#include "Collider.h"
#include "InputManager.h"

// -- Manager -- //
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "GameConfig.h"

#include <DxLib.h>
#include <cmath>

void ButterBlock::Init()
{
    StageBlock::Init();
}

void ButterBlock::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
        return;

    mfAnimTimer += _deltaTime;

    float input = InputManager::GetInstance().GetAxis(Axis::Rotation);
    if (CheckHitKey(KEY_INPUT_L) != 0) input += 1.0f;
    if (CheckHitKey(KEY_INPUT_J) != 0) input -= 1.0f;

    if (input > 0.3f)       mfSlideDir = 1.0f;
    else if (input < -0.3f) mfSlideDir = -1.0f;

    if (Player* pPlayer = FindPlayerOnTop())
    {
        const float stepX = mfSlideDir * SLIDE_SPEED * _deltaTime;

        if (CanSlideHorizontally(pPlayer, stepX))
        {
            if (auto* playerTransform = pPlayer->GetModule<Transform>())
            {
                VECTOR pos = playerTransform->GetPosition();
                pos.x += stepX;
                playerTransform->SetPosition(pos);
                pPlayer->UpdatePieceTransforms();
            }
        }
        else
        {
            mfSlideDir = -mfSlideDir;
        }
    }

    StageBlock::Update(_deltaTime);
}

// バター床の上に乗っているプレイヤーを探す（★末尾の const を削除済み）
Player* ButterBlock::FindPlayerOnTop()
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
    const float blockTop = myPos.y - halfCell;

    for (auto* obj : objectManager->GetObjects())
    {
        if (!obj || obj->GetTag() != Tag::PLAYER_CELL)
            continue;

        auto* piece = static_cast<PlayerPiece*>(obj);
        if (!piece->IsActive())
            continue;

        const VECTOR piecePos = piece->GetModule<Transform>()->GetPosition();
        const float pieceBottom = piecePos.y + halfCell;

        const bool isWithinX = std::fabsf(piecePos.x - myPos.x) <= halfCell;
        const bool isTouchingTop = (pieceBottom >= blockTop - 10.0f) && (pieceBottom <= blockTop + 22.0f);

        if (isWithinX && isTouchingTop)
        {
            return piece->GetPlayer();
        }
    }
    return nullptr;
}

// 滑った先に壁ブロックがないかチェック（★末尾の const を削除済み）
bool ButterBlock::CanSlideHorizontally(Player* pPlayer, float stepX)
{
    auto* scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
    auto* objectManager = scene ? scene->GetObjectManager() : nullptr;
    if (!objectManager)
        return false;

    for (auto* obj : objectManager->GetObjects())
    {
        if (!obj || obj->GetTag() != Tag::PLAYER_CELL)
            continue;

        auto* piece = static_cast<PlayerPiece*>(obj);
        if (!piece->IsActive() || piece->GetPlayer() != pPlayer)
            continue;

        const VECTOR piecePos = piece->GetModule<Transform>()->GetPosition();
        const float nextX = piecePos.x + stepX;

        for (auto* blockObj : objectManager->GetObjects())
        {
            if (!blockObj || blockObj->GetTag() != Tag::BLOCK)
                continue;

            auto* blockCol = blockObj->GetModule<Collider>();
            if (!blockCol || !blockCol->IsEnabled())
                continue;

            const VECTOR blockPos = blockObj->GetModule<Transform>()->GetPosition();

            if (std::fabsf(piecePos.y - blockPos.y) < GameConfig::CELL_SIZE * 0.80f)
            {
                if (std::fabsf(nextX - blockPos.x) < GameConfig::CELL_SIZE * 0.92f)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

// 通常のブロックの上に「とろ?り溶けたバター＆滑る矢印」を重ねて描画
void ButterBlock::Draw()
{
    StageBlock::Draw();

    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return;

    const VECTOR pos = myTransform->GetPosition();
    const int half = static_cast<int>(GameConfig::CELL_SIZE * 0.5f);
    const int left = static_cast<int>(pos.x) - half;
    const int right = static_cast<int>(pos.x) + half;
    const int top = static_cast<int>(pos.y) - half;

    const unsigned int butterMainCol = GetColor(255, 232, 115);
    const unsigned int butterRimCol = GetColor(225, 185, 55);
    const unsigned int shineCol = GetColor(255, 252, 225);

    DrawBox(left - 2, top - 4, right + 2, top + 16, butterMainCol, TRUE);
    DrawLine(left - 2, top + 16, right + 2, top + 16, butterRimCol, 2);

    DrawCircle(left + 18, top + 18, 7, butterMainCol, TRUE);
    DrawCircle(left + 48, top + 22, 9, butterMainCol, TRUE);
    DrawCircle(right - 20, top + 17, 6, butterMainCol, TRUE);

    DrawLine(left + 8, top + 2, right - 12, top + 2, shineCol, 2);

    const int cy = static_cast<int>(pos.y) + 6;
    const int dir = (mfSlideDir >= 0.0f) ? 1 : -1;
    const int slideOffset = static_cast<int>(std::fmodf(mfAnimTimer * 40.0f, 20.0f)) * dir;
    const int cx = static_cast<int>(pos.x) + slideOffset;

    DrawLine(cx - 8 * dir, cy - 10, cx + 6 * dir, cy, butterRimCol, 3);
    DrawLine(cx + 6 * dir, cy, cx - 8 * dir, cy + 10, butterRimCol, 3);
}