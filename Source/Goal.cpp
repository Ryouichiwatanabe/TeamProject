#include "Goal.h"
#include "Player.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

void Goal::Init()
{
    StageBlock::Init();

    // -- 衝突対象「Player」の設定 -- //
    GetModule<Collider>()->AddCollisionTag(Tag::PLAYER);

    // memo : Goalの画像は縦長なので拡縮の必要アリ。
        // Init後にGraphicの初期処理に入ってしまうので、StageのLoad - Goal生成直後に拡縮処理を書いてます
}

bool Goal::IsPlayerTouching()
{
    auto collider = GetModule<Collider>();
    if (!collider)
        return false;

    auto collisions = collider->GetCollisions(Tag::PLAYER_CELL);

    for (auto collision : collisions)
    {
        if (collision)
            return true;
    }

    return false;
}
