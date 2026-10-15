#include "GoalEffect.h"
#include "DxLib.h"
#include <random>
#include <numbers>
#include <cmath>

void GoalEffect::Trigger(float startX, float startY) {
    // 乱数の準備
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> distRotSpeed(-0.08f, 0.08f);

    // 1. 衝撃波リング（中央から一気に広がる光の輪）
    m_waves.push_back(ShockWave{
        .x = startX,
        .y = startY,
        .radius = 5.0f,
        .speed = 10.0f,
        .life = 25,
        .maxLife = 25
        });

    // 2. キラキラ光る粒子（加算合成で破裂する星屑）
    std::uniform_real_distribution<float> distSparkSpeed(3.0f, 9.0f);
    for (int i = 0; i < 50; ++i) {
        float rad = distAngle(rng);
        float spd = distSparkSpeed(rng);

        m_particles.push_back(Particle{
            .x = startX,
            .y = startY,
            .vx = std::cos(rad) * spd,
            .vy = std::sin(rad) * spd,
            .size = 5.0f,
            .color = GetColor(255, 240, 150), // 明るいゴールド
            .life = 40,
            .maxLife = 40,
            .type = 1
            });
    }

    // 3. 紙吹雪（上空に舞い上がってからゆっくり落下）
    const unsigned int confettiColors[] = {
        GetColor(255, 75, 75),   // 赤
        GetColor(255, 215, 0),   // 金
        GetColor(70, 200, 70),   // 緑
        GetColor(60, 160, 255),  // 青
        GetColor(255, 120, 200)  // ピンク
    };
    std::uniform_int_distribution<size_t> distColorIdx(0, std::size(confettiColors) - 1);
    std::uniform_real_distribution<float> distConfettiSpeed(4.0f, 10.0f);
    std::uniform_int_distribution<int> distConfettiLife(90, 160);

    for (int i = 0; i < 120; ++i) {
        float rad = distAngle(rng);
        float spd = distConfettiSpeed(rng);

        // 基本的に上向き（-Y方向）へ噴き出すように調整
        float vx = std::cos(rad) * spd * 0.8f;
        float vy = -std::abs(std::sin(rad) * spd) - 3.0f;

        m_particles.push_back(Particle{
            .x = startX,
            .y = startY,
            .vx = vx,
            .vy = vy,
            .size = 7.0f,
            .angle = rad,
            .rotSpeed = distRotSpeed(rng),
            .color = confettiColors[distColorIdx(rng)],
            .life = distConfettiLife(rng),
            .maxLife = 160,
            .type = 0
            });
    }
}

void GoalEffect::Update() {
    // 衝撃波の更新
    for (auto& wave : m_waves) {
        wave.radius += wave.speed;
        wave.speed *= 0.94f; // 徐々に減速
        --wave.life;
    }

    // パーティクルの更新
    for (auto& p : m_particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.angle += p.rotSpeed;

        if (p.type == 0) {
            // 紙吹雪: 重力と空気抵抗
            p.vy += 0.09f;
            p.vx *= 0.98f;
        }
        else {
            // 光の粒: 急速にブレーキがかかる
            p.vx *= 0.92f;
            p.vy *= 0.92f;
        }

        --p.life;
    }

    // C++20: 寿命が尽きたものをまとめて削除
    std::erase_if(m_waves, [](const ShockWave& w) { return w.life <= 0; });
    std::erase_if(m_particles, [](const Particle& p) { return p.life <= 0; });
}

void GoalEffect::Draw() const {
    // --- 1. 衝撃波の描画（加算合成） ---
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    for (const auto& wave : m_waves) {
        float alphaRate = static_cast<float>(wave.life) / static_cast<float>(wave.maxLife);
        int alpha = static_cast<int>(200 * alphaRate);
        SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

        // 輪郭線として円を描く
        DrawCircle(static_cast<int>(wave.x), static_cast<int>(wave.y), static_cast<int>(wave.radius), GetColor(255, 230, 180), FALSE);
    }

    // --- 2. 光の粒の描画（加算合成） ---
    for (const auto& p : m_particles) {
        if (p.type != 1) continue;

        float alphaRate = static_cast<float>(p.life) / static_cast<float>(p.maxLife);
        int alpha = static_cast<int>(255 * alphaRate);
        SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);

        int r = static_cast<int>(p.size * alphaRate);
        DrawCircle(static_cast<int>(p.x), static_cast<int>(p.y), (r > 1 ? r : 1), p.color, TRUE);
    }

    // --- 3. 紙吹雪の描画（アルファブレンド） ---
    for (const auto& p : m_particles) {
        if (p.type != 0) continue;

        float alphaRate = static_cast<float>(p.life) / static_cast<float>(p.maxLife);
        int alpha = static_cast<int>(255 * alphaRate);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

        // 2Dで「ひらひら舞う」立体感を出すため、cosで横幅を伸縮
        float currentWidth = p.size * std::cos(p.angle);
        float currentHeight = p.size * 0.7f;

        DrawBox(
            static_cast<int>(p.x - currentWidth),
            static_cast<int>(p.y - currentHeight),
            static_cast<int>(p.x + currentWidth),
            static_cast<int>(p.y + currentHeight),
            p.color, TRUE
        );
    }

    // 描画モードを元に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GoalEffect::Clear() {
    m_particles.clear();
    m_waves.clear();
}