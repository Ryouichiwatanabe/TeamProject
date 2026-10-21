#include "Collectible.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

Collectible::Collectible(
    const std::string& _filename,
    int _itemIndex
)
    : mFilename(_filename),
    mItemIndex(_itemIndex)
{
}

void Collectible::Init()
{
    SetTag(Tag::COLLECTIBLE);

    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {
        collider->AddCollisionTag(Tag::PLAYER_CELL);
    }
}

void Collectible::InitComponent()
{
    AddModule<Transform>();
    AddModule<Graphic>(mFilename);
    AddModule<Collider>();

    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {
        collider->SetHalfSize(
            VGet(25.0f, 25.0f, 0.0f)
        );
    }
}

void Collectible::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    auto transform = GetModule<Transform>();

    if (transform == nullptr)
    {
        return;
    }

    // 取得アニメーション中
    if (mbIsPickedUp)
    {
        mPickupTimer += _deltaTime;

        float rate = mPickupTimer / mPickupDuration;

        if (rate > 1.0f)
        {
            rate = 1.0f;
        }

        float scale = 1.0f;

        // 前半：少し大きくなる
        if (rate < 0.5f)
        {
            float expandRate = rate / 0.5f;
            scale = 1.0f + expandRate * 0.3f;
        }
        // 後半：縮んで消える
        else
        {
            float shrinkRate = (rate - 0.5f) / 0.5f;
            scale = 1.3f * (1.0f - shrinkRate);
        }

        auto transform = GetModule<Transform>();

        if (transform != nullptr)
        {
            transform->SetScale(
                VGet(scale, scale, 1.0f)
            );
        }

        if (rate >= 1.0f)
        {
            mbIsCollected = true;
            Destroy();
        }

        return;
    }

    GameObject::Update(_deltaTime);

    auto collider = GetModule<Collider>();

    if (collider == nullptr)
    {
        return;
    }

    // プレイヤーに触れた
    if (collider->IsColliding(Tag::PLAYER_CELL))
    {
        mbIsPickedUp = true;
        mPickupTimer = 0.0f;
    }
}

void Collectible::Draw()
{
    if (mbIsCollected)
    {
        return;
    }

    GameObject::Draw();
}