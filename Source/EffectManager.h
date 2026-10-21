#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include "EffectBase.h"

class EffectManager final
{
public:
    EffectManager() = default;
    ~EffectManager();

    // 単一管理クラスとしてコピー・ムーブを禁止
    EffectManager(const EffectManager&) = delete;
    EffectManager& operator=(const EffectManager&) = delete;
    EffectManager(EffectManager&&) noexcept = delete;
    EffectManager& operator=(EffectManager&&) noexcept = delete;

    // 初期化と終了処理
    bool Initialize(int screenWidth, int screenHeight);
    void Terminate();

    // 毎フレームの更新と描画（deltaTime を受け取る）
    void Update(float deltaTime);
    void Draw() const;

    // エフェクト再生（名前指定でインスタンスを生成・登録）
    void PlayEffect(std::string_view name, float x, float y);

    // すべての再生中エフェクトを強制クリア
    void ClearAll();

private:
    bool isInitialized_{ false };
    int screenWidth_{ 0 };
    int screenHeight_{ 0 };

    // 再生中エフェクトインスタンスの所有（RAII / コンポジション）
    std::vector<std::unique_ptr<EffectBase>> activeEffects_;
};