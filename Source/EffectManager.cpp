#include "EffectManager.h"

void EffectManager::Update(float deltaTime)
{
    // 更新中に Play() で effects_ が増えても安全なインデックス走査
    for (size_t i = 0; i < effects_.size(); ++i)
    {
        effects_[i]->Update(deltaTime);
    }

    // IsDead() が true を返したエフェクトを一括削除
    std::erase_if(effects_, [](const std::unique_ptr<EffectBase>& effect) {
        return effect->IsDead();
        });

    if (mpEffectClick)
    {
        mpEffectClick->Update(deltaTime);
    }
   
}

//unique_ptr   Delete漏れを防ぐ

void EffectManager::Draw() const
{
    for (const auto& effect : effects_)
    {
        effect->Draw();
    }
    if (mpEffectClick)
    {
        mpEffectClick->Draw();
    }
   

}


void EffectManager::Clear()
{
    effects_.clear();
    delete mpEffectClick;
    mpEffectClick = nullptr;

}