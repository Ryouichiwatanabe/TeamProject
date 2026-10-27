#include "EffectManager.h"
#include "InputManager.h"
#include "SoundManager.h"

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

 
   
}

//unique_ptr   Delete漏れを防ぐ

void EffectManager::Draw() const
{
    for (const auto& effect : effects_)
    {
        effect->Draw();
    }
   
   

}


void EffectManager::Clear()
{
    effects_.clear();

}

// クリック状態を取得してエフェクトを生成する処理
// どのシーンでも使用するからここで呼び出し
void EffectManager::PlayClickRipple()
{
    auto& inputManager = InputManager::GetInstance();
    if (inputManager.GetButtonDown(Button::Confirm))
    {
        const auto& mouse = inputManager.GetMouse();
        const float posX = static_cast<float>(mouse.GetX());
        const float posY = static_cast<float>(mouse.GetY());

        // ClickEffect を生成して effects_ に追加
        effects_.emplace_back(std::make_unique<EffectClick>(posX, posY));
        // SE
        SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_DECIDE);
    }
}
