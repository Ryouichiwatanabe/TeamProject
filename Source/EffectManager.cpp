#include "EffectManager.h"
#include "EffectClick.h"
#include <algorithm>

EffectManager::~EffectManager()
{
    Terminate();
}

bool EffectManager::Initialize(int screenWidth, int screenHeight)
{
    if (isInitialized_)
    {
        return true;
    }

    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;

    isInitialized_ = true;
    return true;
}

void EffectManager::Terminate()
{
    if (!isInitialized_)
    {
        return;
    }

    ClearAll();
    isInitialized_ = false;
}

void EffectManager::Update(float deltaTime)
{
    if (!isInitialized_)
    {
        return;
    }

    // 各エフェクトの更新
    for (auto& effect : activeEffects_)
    {
        effect->Update(deltaTime);
    }

    // C++20: 寿命が尽きたエフェクトを一括安全削除（メモリも自動解放）
    std::erase_if(activeEffects_, [](const std::unique_ptr<EffectBase>& effect) {
        return !effect || effect->IsDead();
        });
}

void EffectManager::Draw() const
{
    if (!isInitialized_)
    {
        return;
    }

    // 登録されているエフェクトを順次描画
    for (const auto& effect : activeEffects_)
    {
        if (effect && !effect->IsDead())
        {
            effect->Draw();
        }
    }
}

void EffectManager::PlayEffect(std::string_view name, float x, float y)
{
    if (!isInitialized_)
    {
        return;
    }

    // 名前判定で対応するエフェクトクラスを生成（簡易ファクトリ）
    if (name == "EffectClick")
    {
        activeEffects_.emplace_back(std::make_unique<EffectClick>(x, y));
    }
}

void EffectManager::ClearAll()
{
    activeEffects_.clear();
}