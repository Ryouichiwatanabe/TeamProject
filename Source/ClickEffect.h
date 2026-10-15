#pragma once

#include <vector>
#include <random>

namespace TofuGame {

    struct Vector2 {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Color {
        float r = 0.98f;
        float g = 0.98f;
        float b = 0.94f;
        float a = 1.0f;
    };

    // 飛散する豆腐の角切り破片
    struct DebrisParticle {
        Vector2 position;
        Vector2 velocity;
        float rotation = 0.0f;
        float angularVelocity = 0.0f;
        float size = 8.0f;
        float currentLifetime = 0.0f;
        float maxLifetime = 0.5f;
        Color color;

        [[nodiscard]] bool isDead() const noexcept {
            return currentLifetime >= maxLifetime;
        }

        [[nodiscard]] float getNormalizedLife() const noexcept {
            return (maxLifetime > 0.0f) ? (currentLifetime / maxLifetime) : 1.0f;
        }
    };

    // 豆乳の飛沫・衝撃波リング
    struct ShockwaveRing {
        Vector2 position;
        float currentRadius = 6.0f;
        float targetRadius = 45.0f;
        float thickness = 3.5f;
        float currentLifetime = 0.0f;
        float maxLifetime = 0.35f;

        [[nodiscard]] bool isDead() const noexcept {
            return currentLifetime >= maxLifetime;
        }

        [[nodiscard]] float getNormalizedLife() const noexcept {
            return (maxLifetime > 0.0f) ? (currentLifetime / maxLifetime) : 1.0f;
        }
    };

    class ClickEffect {
    public:
        ClickEffect();
        ~ClickEffect() = default;

        ClickEffect(const ClickEffect&) = delete;
        ClickEffect& operator=(const ClickEffect&) = delete;
        ClickEffect(ClickEffect&&) noexcept = default;
        ClickEffect& operator=(ClickEffect&&) noexcept = default;

        /// @brief クリック地点に豆腐エフェクトを生成
        void spawn(const Vector2& clickPosition);

        /// @brief 毎フレームの物理・寿命計算
        /// @param deltaTime 経過秒数 (通常 1.0f / 60.0f)
        void update(float deltaTime);

        /// @brief DxLibを使った描画処理
        void draw() const;

        /// @brief 画面遷移時などのリセット
        void clear() noexcept;

        [[nodiscard]] const std::vector<DebrisParticle>& getDebrisParticles() const noexcept { return m_debrisParticles; }
        [[nodiscard]] const std::vector<ShockwaveRing>& getShockwaves() const noexcept { return m_shockwaves; }

    private:
        std::vector<DebrisParticle> m_debrisParticles;
        std::vector<ShockwaveRing> m_shockwaves;

        std::mt19937 m_rng;

        static constexpr float Gravity = 480.0f;
        static constexpr float AirResistance = 0.94f;
        static constexpr int DebrisSpawnCount = 10;
    };

} // namespace TofuGame