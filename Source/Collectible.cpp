#include "Collectible.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

void Collectible::Init()
{
    // 取得アイテムのタグを設定
    SetTag(Tag::COLLECTIBLE);

    // プレイヤーとの衝突を検知する
    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {
        collider->AddCollisionTag(Tag::PLAYER_CELL);
    }
}

void Collectible::InitComponent()
{
    // 座標・回転・拡大率
    AddModule<Transform>();

    // 取得アイテムの画像
    // ※画像パスは実際のファイル名に合わせて変更する
    AddModule<Graphic>("Resource/Cursor.png");

    // 当たり判定
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
    // ポーズ中は処理しない
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    // 取得済みなら何もしない
    if (mbIsCollected)
    {
        return;
    }

    // GameObjectの更新
    GameObject::Update(_deltaTime);

    auto collider = GetModule<Collider>();
    if (collider == nullptr)
    {
        return;
    }

    // プレイヤーに触れたら取得
    if (collider->IsColliding(Tag::PLAYER_CELL))
    {
        mbIsCollected = true;
        Destroy();
    }
}

void Collectible::Draw()
{
    // 取得済みなら描画しない
    if (mbIsCollected)
    {
        return;
    }

    GameObject::Draw();
}