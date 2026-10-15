#include "Goal.h"
#include "Player.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

void Goal::Init()
{
    StageBlock::Init();
    // プレイヤーとの衝突を見る
    GetModule<Collider>()->AddCollisionTag(Tag::PLAYER);
}

bool Goal::IsPlayerTouching()
{
    auto collider = GetModule<Collider>();

    if (collider == nullptr)
        return false;

    return collider->IsColliding(Tag::PLAYER_CELL);
}

bool Goal::IsShapeMatched(Player* _player) const
{
    if (_player == nullptr)
        return false;

    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            bool isPieceEnabled =
                _player->IsPieceEnabled(x, y);

            if (isPieceEnabled != mShape[y][x])
                return false;
        }
    }

    return true;
}

bool Goal::IsWithinDistance(Player* _player)
{
    if (_player == nullptr)
        return false;

    auto playerTransform =
        _player->GetModule<Transform>();

    auto goalTransform =
        GetModule<Transform>();

    if (playerTransform == nullptr ||
        goalTransform == nullptr)
        return false;

    VECTOR playerPos =
        playerTransform->GetPosition();

    VECTOR goalPos =
        goalTransform->GetPosition();

    float distance =
        VSize(VSub(playerPos, goalPos));

    const float clearDistance = 50.0f;

    return distance <= clearDistance;
}
