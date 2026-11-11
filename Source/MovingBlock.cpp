#include "MovingBlock.h"
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

void MovingBlock::Init()
{
    StageBlock::Init(); // Tag::BLOCK として登録
}

void MovingBlock::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
        return;

    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return;

    // 1. 初回フレームで「CSVで配置された元の座標」を記憶する
    if (!mbStartPosSaved)
    {
        mvStartPos = myTransform->GetPosition();
        mbStartPosSaved = true;
    }

    // 2. 移動する前の座標と、今上に乗っているプレイヤーを先に取得しておく
    const VECTOR prevPos = myTransform->GetPosition();
    Player* pRidingPlayer = FindPlayerOnTop(prevPos);

    // 3. サイン波（std::sinf）を使って、端で減速しながら滑らかに往復移動させる
    mfMoveTimer += _deltaTime * MOVE_SPEED;
    const float offset = std::sinf(mfMoveTimer) * MOVE_RANGE;

    VECTOR nextPos = mvStartPos;
    if (IS_VERTICAL_MOVE)
    {
        nextPos.y += offset; // 上下移動モード
    }
    else
    {
        nextPos.x += offset; // 左右移動モード
    }

    myTransform->SetPosition(nextPos);

    // 4. このフレームで床が動いた差分（moveDelta）だけ、上に乗っているプレイヤーも一緒に運ぶ！
    const VECTOR moveDelta = VSub(nextPos, prevPos);
    // �v���C���[���ό`��ʂ��J���Ă����(!pRidingPlayer->IsShapeEditing())�͓������Ȃ�
    if (pRidingPlayer != nullptr && !pRidingPlayer->IsShapeEditing())
    {
        if (auto* playerTransform = pRidingPlayer->GetModule<Transform>())
        {
            const VECTOR playerPos = playerTransform->GetPosition();
            playerTransform->SetPosition(VAdd(playerPos, moveDelta));
            pRidingPlayer->UpdatePieceTransforms();
        }
    }

    StageBlock::Update(_deltaTime);
}

// この動く床の上に乗っているプレイヤーを探す
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

        // ピースが床の横幅の範囲内で、かつ下端が床の上面に触れているか
        const bool isWithinX = std::fabsf(piecePos.x - blockPos.x) < (GameConfig::CELL_SIZE * 0.90f);
        const bool isTouchingTop = (pieceBottom >= blockTop - 14.0f) && (pieceBottom <= blockTop + 26.0f);

        if (isWithinX && isTouchingTop)
        {
            return piece->GetPlayer();
        }
    }
    return nullptr;
}

// 「動くまな板リフト（ガイドレール＋木製カッティングボード）」の描画
void MovingBlock::Draw()
{
    auto* myTransform = GetModule<Transform>();
    if (!myTransform)
        return;

    const VECTOR pos = myTransform->GetPosition();
    const int cx = static_cast<int>(pos.x);
    const int cy = static_cast<int>(pos.y);
    const int half = static_cast<int>(GameConfig::CELL_SIZE * 0.5f);
    const int left = cx - half;
    const int right = cx + half;
    const int top = cy - half;
    const int bottom = cy + half;

    const unsigned int railCol = GetColor(130, 138, 145); // 背後のガイドレール色
    const unsigned int shadowCol = GetColor(20, 18, 15);    // 立体影
    const unsigned int woodMainCol = GetColor(225, 178, 120); // まな板の明るい木目色
    const unsigned int woodDarkCol = GetColor(155, 108, 62);  // まな板の側面・枠色
    const unsigned int metalRimCol = GetColor(195, 205, 215); // 両端のステンレス金具色
    const unsigned int arrowCol = GetColor(125, 82, 42);   // 中央の移動方向マーク

    // 2. ブロックの影
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 75);
    DrawBox(left + 6, top + 8, right + 6, bottom + 8, shadowCol, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 3. 木製まな板ブロック本体と下側の厚み
    DrawBox(left, top, right, bottom, woodMainCol, TRUE);
    DrawBox(left, bottom - 14, right, bottom, woodDarkCol, TRUE);
    DrawBox(left, top, right, bottom, woodDarkCol, FALSE);

    // 4. 左右両端のステンレス補強金具とリベット
    DrawBox(left, top, left + 12, bottom, metalRimCol, TRUE);
    DrawBox(right - 12, top, right, bottom, metalRimCol, TRUE);
    DrawBox(left, top, left + 12, bottom, woodDarkCol, FALSE);
    DrawBox(right - 12, top, right, bottom, woodDarkCol, FALSE);
    DrawCircle(left + 6, cy, 3, woodDarkCol, TRUE);
    DrawCircle(right - 6, cy, 3, woodDarkCol, TRUE);

    // 5. 中央の「＜ ＞」移動マーク
    DrawLine(cx - 18, cy - 4, cx - 26, cy - 12, arrowCol, 3);
    DrawLine(cx - 18, cy - 4, cx - 26, cy + 4, arrowCol, 3);
    DrawLine(cx + 18, cy - 4, cx + 26, cy - 12, arrowCol, 3);
    DrawLine(cx + 18, cy - 4, cx + 26, cy + 4, arrowCol, 3);
}