#include "Goal.h"
#include "Player.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

void Goal::Init()
{
    SetTag(Tag::BLOCK);

    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {
        collider->AddCollisionTag(Tag::PLAYER_CELL);
    }
}

void Goal::InitComponent()
{
    AddModule<Transform>();
    AddModule<Graphic>(GetModelFilename());
    AddModule<Collider>();
}

void Goal::Update(float _deltaTime)
{
    GameObject::Update(_deltaTime);
}

void Goal::Draw()
{
    GameObject::Draw();
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
