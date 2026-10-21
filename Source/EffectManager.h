#pragma once
#include <vector>
#include <memory>
#include <utility>
#include "EffectBase.h"
#include "EffectClick.h"

//class EffectClick

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

    /**
     * @brief 任意のエフェクトを生成して再生リストに追加
     * 使用例: effectManager->Play<ClickEffect>(x, y);
     */
    template <typename T, typename... Args>
    T* Play(Args&&... args)
    {
        auto effect = std::make_unique<T>(std::forward<Args>(args)...);
        T* rawPtr = effect.get(); // 戻り値で受け取れば即座に操作も可能
        effects_.push_back(std::move(effect));
        return rawPtr;
    }


    void SpawnClickEffect(float x, float y)
    {
        // 既に存在していれば、古いものを削除してメモリリークを防ぐ
        delete mpEffectClick;
        mpEffectClick = new EffectClick(x, y);
    }


private:
    std::vector<std::unique_ptr<EffectBase>> effects_;

    EffectClick* mpEffectClick;

};