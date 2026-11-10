#include "Spike.h"
#include "Player.h"
#include "PlayerPiece.h"

// -- Module -- //
#include "Transform.h"
#include "Collider.h"

// -- Resource -- //
#include "Master.h"
#include "ResourceManager.h"
#include "GameConfig.h"

void Spike::InitComponent()
{
    AddModule<Transform>();
    AddModule<Collider>();

    // プレイヤーのピースとの当たり判定を設定
    if (auto* collider = GetModule<Collider>())
    {
        collider->AddCollisionTag(Tag::PLAYER_CELL);
        collider->SetHalfSize(VGet(GameConfig::CELL_SIZE / 4.0f, 1.0f, 0.0f));
    }
}

void Spike::Init()
{
    SetTag(Tag::SPIKE);
}

void Spike::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    // 初回更新時に自分の座標へスプライトシートを生成
    if (!mpSmokeEffect)
    {
        if (auto* transform = GetModule<Transform>())
        {
            const VECTOR pos = transform->GetPosition();
            auto* pSmokeData = Master::mpResource->LoadDivGraphics("Resource/Smoke2.png", 16, 4, 4);

            if (pSmokeData != nullptr)
            {
                mpSmokeEffect = std::make_unique<SpriteSheet>(
                    pos.x,          // X座標
                    pos.y + 20.0f,  // Y座標（床に接地するよう微調整）
                    pSmokeData,     // 分割画像
                    0.05f,          // 1コマの秒数
                    true,           // ループ再生
                    0.8             // 拡大率
                );
            }
        }
    }

    // アニメーション時間を進める
    if (mpSmokeEffect)
    {
        mpSmokeEffect->Update(_deltaTime);
    }

    StageBlock::Update(_deltaTime);

    CheckPlayerCollision();
}

void Spike::Draw()
{
    // スプライトシートを描画
    if (mpSmokeEffect)
    {
        mpSmokeEffect->Draw();
    }
}

void Spike::CheckPlayerCollision()
{
    auto* collider = GetModule<Collider>();
    if (collider == nullptr)
    {
        return;
    }

    // 有効なプレイヤーピースが触れたらリスポーンさせる
    const auto& collisions = collider->GetCollisions(Tag::PLAYER_CELL);

    for (auto* object : collisions)
    {
        if (object == nullptr)
        {
            continue;
        }

        auto* piece = static_cast<PlayerPiece*>(object);
        if (!piece->IsActive())
        {
            continue;
        }

        if (Player* player = piece->GetPlayer())
        {
            player->Respawn();
            break;
        }
    }
}