#include "Collectible.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

Collectible::Collectible(const std::string& _filename,int _itemIndex)
    : mFilename(_filename),
    mItemIndex(_itemIndex)
{
}

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
    AddModule<Graphic>(mFilename);

    // 当たり判定
    AddModule<Collider>();

    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {

        collider->SetHalfSize(VGet(25.0f, 25.0f, 0.0f));
    
    }
}

void Collectible::Update(float _deltaTime)
{
    // ポーズ中は処理しない
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    // 取得演出中
    if (mbIsCollectEffect)
    {
        mCollectTime += _deltaTime;

        auto transform = GetModule<Transform>();
        if (transform != nullptr)
        {
            float progress =
                mCollectTime / mCollectDuration;

            float scale = 1.0f;

            // 前半：少し大きくなる
            if (progress < 0.35f)
            {
                float effectProgress =
                    progress / 0.35f;

                scale = 1.0f + effectProgress * 0.15f;
            }
            // 後半：小さくなって消える
            else
            {
                float effectProgress =
                    (progress - 0.35f) / 0.65f;

                scale = 1.15f * (1.0f - effectProgress);
            }

            transform->SetScale(
                VGet(scale, scale, 1.0f)
            );
        }

        // 演出終了
        if (mCollectTime >= mCollectDuration)
        {
            mbIsCollected = true;
            Destroy();
        }

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

    // プレイヤーに触れたら取得演出開始
    if (collider->IsColliding(Tag::PLAYER_CELL))
    {
        mbIsPickedUp = true;
        mbIsCollectEffect = true;
        mCollectTime = 0.0f;

        return;
    }

    // アイテムの上下移動
    mMoveTime += _deltaTime;

    auto transform = GetModule<Transform>();
    if (transform != nullptr)
    {
        VECTOR position = transform->GetPosition();

        // 最初の位置を上下移動の基準位置として保存
        if (!mbIsBasePositionSet)
        {
            mBaseY = position.y;
            mbIsBasePositionSet = true;
        }

        position.y =
            mBaseY + sinf(mMoveTime * 3.0f) * 5.0f;

        transform->SetPosition(position);
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