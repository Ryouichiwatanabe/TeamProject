#include "EffectRespawn.h"
#include "Player.h"
#include "PlayerPiece.h"
#include "Graphic.h"
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"

EffectRespawn::EffectRespawn(Player* pPlayer)
    : EffectBase(0.0f, 0.0f, LIFE_TIME)
    , m_pPlayer(pPlayer)
{
    if (m_pPlayer == nullptr)
    {
        m_bIsDead = true;
    }
}

EffectRespawn::~EffectRespawn()
{
    // エフェクト終了時に必ずブロックを元の表示状態へ戻す
    SetPlayerBlocksVisible(true);
}

void EffectRespawn::OnUpdate(float fDeltaTime)
{
    if (m_bIsDead || m_pPlayer == nullptr)
        return;

    // 寿命が尽きる直前は必ず表示状態に戻して終える
    if (m_fElapsedTime + fDeltaTime >= m_fLifeTime)
    {
        SetPlayerBlocksVisible(true);
        return;
    }

    // 経過時間を BLINK_INTERVAL で割って、偶数・奇数で表示／非表示をパチパチ切り替える
    const int blinkPhase = static_cast<int>(m_fElapsedTime / BLINK_INTERVAL);
    const bool isVisible = (blinkPhase % 2 == 0);

    SetPlayerBlocksVisible(isVisible);
}

void EffectRespawn::Draw() const
{
    // ブロック自体の表示/非表示を切り替えているため、余計な図形は何も描かない
}

void EffectRespawn::SetPlayerBlocksVisible(bool bVisible) const
{
    auto* scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
    auto* objectManager = scene ? scene->GetObjectManager() : nullptr;
    if (!objectManager)
        return;

    for (auto* obj : objectManager->GetObjects())
    {
        if (!obj || obj->GetTag() != Tag::PLAYER_CELL)
            continue;

        auto* piece = static_cast<PlayerPiece*>(obj);
        if (piece->GetPlayer() != m_pPlayer)
            continue;

        // 有効なピース（現在の形のブロック）だけ、点滅に合わせて Graphic の表示/非表示を切り替える
        if (auto* graphic = piece->GetModule<Graphic>())
        {
            graphic->SetEnabled(piece->IsActive() ? bVisible : false);
        }
    }
}