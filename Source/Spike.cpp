#include "Spike.h"
#include "Player.h"

// -- Module -- //
#include "Transform.h"
#include "Collider.h"
#include "Gravity.h"

void Spike::Init()
{
    SetTag(Tag::SPIKE);     // タグ設定
    GetModule<Collider>()->AddCollisionTag(Tag::PLAYER);    // 衝突対象を設定
  
    GetModule<Gravity>()->SetEnabled(false);    // 重力を無効化
}

void Spike::InitComponent()
{
    AddModule<Transform>();
    AddModule<Collider>();
    AddModule<Gravity>();
}

void Spike::Update(float _deltaTime)
{
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    CheckPlayerCollision();

    GameObject::Update(_deltaTime);
}

void Spike::Draw()
{
    GameObject::Draw();
}

void Spike::CheckPlayerCollision()
{
    auto collisions = GetModule<Collider>()->GetCollisions(Tag::PLAYER);

    for (auto* playerObject : collisions)
    {
        if (playerObject == nullptr)
        {
            continue;
        }

        Player* player = dynamic_cast<Player*>(playerObject);

        if (player == nullptr)
        {
            continue;
        }

        // Playerをリスポーン位置へ戻す
        // player->Respawn();

        break;
    }
}