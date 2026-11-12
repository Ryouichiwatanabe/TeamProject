#include "MovingBlock.h"
#include "Player.h"
#include "PlayerPiece.h"

#include "Transform.h"
#include "Collider.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "GameConfig.h"

#include <DxLib.h>
#include <cmath>

void MovingBlock::Init()
{
    StageBlock::Init();
}

void MovingBlock::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
        return;

    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return;

    // 初期座標を記憶
    if (!mbStartPosSaved)
    {
        mvStartPos = myTransform->GetPosition();
        mbStartPosSaved = true;
    }

    // 移動前の座標と、上に乗っているプレイヤーを取得
    const VECTOR prevPos = myTransform->GetPosition();
    Player* pRidingPlayer = FindPlayerOnTop(prevPos);

    // 往復移動の座標計算
    mfMoveTimer += _deltaTime * MOVE_SPEED;
    const float offset = std::sinf(mfMoveTimer) * MOVE_RANGE;

    VECTOR nextPos = mvStartPos;
    if (IS_VERTICAL_MOVE)
        nextPos.y += offset;
    else
        nextPos.x += offset;

    myTransform->SetPosition(nextPos);

    // 乗っているプレイヤーを一緒に移動（変形中は除外）
    const VECTOR moveDelta = VSub(nextPos, prevPos);
    if (pRidingPlayer != nullptr && !pRidingPlayer->IsShapeEditing())
    {
        if (auto* playerTransform = pRidingPlayer->GetModule<Transform>())
        {
            playerTransform->SetPosition(VAdd(playerTransform->GetPosition(), moveDelta));
            pRidingPlayer->UpdatePieceTransforms();
        }
    }

    StageBlock::Update(_deltaTime);
}

Player* MovingBlock::FindPlayerOnTop(const VECTOR& blockPos)
{
    auto* scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
    auto* objectManager = scene ? scene->GetObjectManager() : nullptr;
    if (!objectManager)
        return nullptr;

    const float halfCell = GameConfig::CELL_SIZE * 0.5f;
    const float blockTop = blockPos.y - halfCell;

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

        // X座標が重なっており、下面がブロックの上面に接しているか判定
        const bool isWithinX = std::fabsf(piecePos.x - blockPos.x) < (GameConfig::CELL_SIZE * 0.90f);
        const bool isTouchingTop = (pieceBottom >= blockTop - 14.0f) && (pieceBottom <= blockTop + 26.0f);

        if (isWithinX && isTouchingTop)
            return piece->GetPlayer();
    }
    return nullptr;
}

void MovingBlock::Draw()
{
    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return;

    // ブロック本体の描画（GetModelFilenameの画像）
    StageBlock::Draw();
}