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
    // StageBlock::InitComponent() を呼ぶと静止画の Graphic モジュールまで追加されるため、
    // Transform と Collider のみ追加する（もしくは StageBlock::InitComponent() を呼んで Graphic を SetEnabled(false) にする）
    AddModule<Transform>();
    AddModule<Collider>();

    if (auto* collider = GetModule<Collider>())
    {
        // PlayerPiece (Tag::PLAYER_CELL) との衝突を検知する
        collider->AddCollisionTag(Tag::PLAYER_CELL);
        collider->SetHalfSize(VGet(GameConfig::CELL_SIZE / 2.0f, GameConfig::CELL_SIZE / 2.0f, 0.0f));
    }
}

void Spike::Init()
{
    // StageBlock::Init() の Tag::BLOCK ではなく Tag::SPIKE に設定
    SetTag(Tag::SPIKE);
}

void Spike::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    // 初回更新時に Transform の座標を取得して EffectSmokeSpriteSheet を生成
    // (Stage::CreateObjects で CreateObject 後に SetPosition されるため)
    if (!mpSmokeEffect)
    {
        if (auto* transform = GetModule<Transform>())
        {
            const VECTOR pos = transform->GetPosition();
            auto* pSmokeData = Master::mpResource->LoadDivGraphics("Resource/Smoke2.png", 16, 4, 4);

            if (pSmokeData != nullptr)
            {
                mpSmokeEffect = std::make_unique<EffectSmokeSpriteSheet>(
                    pos.x,
                    pos.y,
                    pSmokeData,
                    0.05f,          // 1コマの表示秒数
                    true,           // ループ再生
                    false,          // アルファブレンド
                    0.8,            // ★スケール（1マス=100pxに合わせるなら 0.7〜0.8 くらいが見栄え◎）
                    0.0,            // 角度
                    255, 255, 255, 255,
                    -1.0f,          // ★fPivotX: -1.0f にすると自動で1コマの横中心になります
                    -1.0f           // ★fPivotY: -1.0f にすると自動で1コマの縦中心になります
                );
            }
        }
    }

    // スプライトシートのアニメーション時間を進める
    if (mpSmokeEffect)
    {
        mpSmokeEffect->Update(_deltaTime);
    }

    StageBlock::Update(_deltaTime);

    CheckPlayerCollision();
}

void Spike::Draw()
{
    // StageBlock::Draw() は呼ばず（または Graphic を持たせず）、スプライトシートを描画
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

    // PlayerPiece (Tag::PLAYER_CELL) との衝突リストを取得
    const auto& collisions = collider->GetCollisions(Tag::PLAYER_CELL);

    for (auto* object : collisions)
    {
        if (object == nullptr)
        {
            continue;
        }

        // PlayerPiece から親の Player を取得してリスポーンさせる
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