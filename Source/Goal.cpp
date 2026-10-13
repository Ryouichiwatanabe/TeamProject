#include "Goal.h"
#include "Player.h"

#include "Transform.h"
#include "Collider.h"

void Goal::Init()
{
    StageBlock::Init();
    // プレイヤーとの衝突を見る
    GetModule<Collider>()->AddCollisionTag(Tag::PLAYER);
}

bool Goal::IsPlayerTouching()
{
	return GetModule<Collider>()->IsColliding(Tag::PLAYER);
}

bool Goal::IsShapeMatched(Player* _player) const
{
    if (_player == nullptr)
        return false;

    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            bool isPieceEnabled = _player->IsPieceEnabled(x, y);
            if (isPieceEnabled != mShape[y][x])
            {
                return false;
            }
        }
    }

    return true;
}

bool Goal::IsWithinDistance(Player* _player)
{
    if (_player == nullptr)
        return false;

    VECTOR playerPos = _player->GetModule<Transform>()->GetPosition();
    VECTOR goalPos = GetModule<Transform>()->GetPosition();

    float distance = VSize(
        VSub(playerPos, goalPos)
    );

    const float clearDistance = 100.0f;

    return distance <= clearDistance;
}