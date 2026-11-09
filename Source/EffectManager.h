#pragma once
#include <vector>
#include <memory>
#include <utility>
#include "EffectBase.h"
#include "EffectClick.h"

class EffectManager final
{
public:
    EffectManager() = default;
    ~EffectManager() = default;

    // コピー・ムーブ禁止
    EffectManager(const EffectManager&) = delete;
    EffectManager& operator=(const EffectManager&) = delete;

    void Update(float deltaTime);
    void Draw() const;
    void Clear();
    void DrawBackground() const; // ★追加：背面エフェクトの描画
    void PlayClickRipple();

    /**
     * @brief 任意のエフェクトを生成して再生リストに追加
     * 使用例: effectManager->Play<ClickEffect>(x, y);
     */
    template <typename T, typename... Args>
    T* Play(Args&&... args)
    {
        auto effect = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = effect.get();
        effects_.push_back(std::move(effect));
        return rawPtr;
    }

private:
    std::vector<std::unique_ptr<EffectBase>> effects_;
};