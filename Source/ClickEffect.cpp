#include "ClickEffect.h"
#include <DxLib.h>

#include <cmath>
#include <numbers>
#include <algorithm>

namespace TofuGame {

    ClickEffect::ClickEffect()
        // 乱数生成器 (std::mt19937 など) をハードウェア乱数でシード初期化
        : m_rng(std::random_device{}()) {
        // 動的リサイズによるヒープ再確保のオーバーヘッドを避けるため、事前にメモリを確保
        m_debrisParticles.reserve(128);
        m_shockwaves.reserve(16);
    }

    void ClickEffect::spawn(const Vector2& clickPosition) {
        // パーティクルの初期パラメータ用の一様分布を定義
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * std::numbers::pi_v<float>);
        std::uniform_real_distribution<float> speedDist(100.0f, 220.0f);
        std::uniform_real_distribution<float> sizeDist(6.0f, 12.0f);
        std::uniform_real_distribution<float> rotSpeedDist(-8.0f, 8.0f);
        std::uniform_real_distribution<float> lifeDist(0.4f, 0.6f);

        // -------------------------------------------------------------------------
        // 1. 豆腐破片（角切りキューブ）の生成
        // -------------------------------------------------------------------------
        for (int i = 0; i < DebrisSpawnCount; ++i) {
            const float angle = angleDist(m_rng);
            const float speed = speedDist(m_rng);

            // 指示付き初期化子 (C++20) で各パラメータを明示的にセット
            m_debrisParticles.push_back(DebrisParticle{
                .position = clickPosition,
                // 上方向に少し跳ね上がるように Y 初速に -60.0f のオフセットを付与
                .velocity = { std::cos(angle) * speed, std::sin(angle) * speed - 60.0f },
                .angularVelocity = rotSpeedDist(m_rng), // 毎秒の回転角度 (ラジアン)
                .size = sizeDist(m_rng),
                .currentLifetime = 0.0f,
                .maxLifetime = lifeDist(m_rng),
                .color = { 0.96f, 0.96f, 0.92f, 1.0f } // 豆腐のベースカラー (アイボリー)
                });
        }

        // -------------------------------------------------------------------------
        // 2. 豆乳リング（クリック時の波紋エフェクト）の生成
        // -------------------------------------------------------------------------
        m_shockwaves.push_back(ShockwaveRing{
            .position = clickPosition,
            .currentRadius = 6.0f,   // 開始半径
            .targetRadius = 45.0f,   // 到達する最大半径
            .thickness = 3.5f,       // 線の太さ表現用
            .currentLifetime = 0.0f,
            .maxLifetime = 0.3f
            });
    }

    void ClickEffect::update(float deltaTime) {
        // -------------------------------------------------------------------------
        // 1. 破片の物理挙動更新
        // -------------------------------------------------------------------------
        for (auto& debris : m_debrisParticles) {
            debris.currentLifetime += deltaTime;

            // 下向きの重力を加算
            debris.velocity.y += Gravity * deltaTime;

            // 【改善点】フレームレート非依存の空気抵抗減衰
            // std::pow(減衰率, deltaTime) により 60fps/144fps でも同じ減衰率を維持
            debris.velocity.x *= std::pow(AirResistance, deltaTime);

            // 速度から座標を更新 (オイラー積分)
            debris.position.x += debris.velocity.x * deltaTime;
            debris.position.y += debris.velocity.y * deltaTime;

            // 【改善点】角速度から回転角度を加算（draw で実際に回転描画される）
            debris.rotation += debris.angularVelocity * deltaTime;

            // 寿命の進行度に応じてアルファ値（透明度）を線形フェードアウト
            debris.color.a = std::clamp(1.0f - debris.getNormalizedLife(), 0.0f, 1.0f);
        }

        // -------------------------------------------------------------------------
        // 2. 衝撃波リングの拡大更新
        // -------------------------------------------------------------------------
        for (auto& wave : m_shockwaves) {
            wave.currentLifetime += deltaTime;

            // 生存時間を 0.0f ～ 1.0f に正規化
            const float progress = std::clamp(wave.getNormalizedLife(), 0.0f, 1.0f);

            // sin 関数を用いたイージング (Ease-Out) で、最初は素早く広がり徐々に減速
            wave.currentRadius = 6.0f + (wave.targetRadius - 6.0f) * std::sin(progress * (std::numbers::pi_v<float> *0.5f));
        }

        // -------------------------------------------------------------------------
        // 3. 寿命を迎えた要素の削除 (C++20 std::erase_if)
        // -------------------------------------------------------------------------
        // イテレータの無効化を気にせず、O(N) で不要な破片・リングを安全に一括消去
        std::erase_if(m_debrisParticles, [](const DebrisParticle& p) noexcept { return p.isDead(); });
        std::erase_if(m_shockwaves, [](const ShockwaveRing& w) noexcept { return w.isDead(); });
    }

    void ClickEffect::draw() const {
        // 【改善点】ループ毎の GetColor 計算オーバーヘッドを防ぐため、色コードを静的キャッシュ
        static const unsigned int RingColor = GetColor(245, 245, 235); // 豆乳リング色
        static const unsigned int DebrisColor = GetColor(250, 250, 242); // 豆腐塗りつぶし色
        static const unsigned int DebrisOutline = GetColor(210, 210, 200); // 豆腐の境界線色

        // -------------------------------------------------------------------------
        // 1. 豆乳リングの描画
        // -------------------------------------------------------------------------
        for (const auto& wave : m_shockwaves) {
            // 時間経過に伴ってアルファ値をフェードアウト
            const int alpha = static_cast<int>((1.0f - wave.getNormalizedLife()) * 220.0f);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

            // 【改善点】thickness（線の太さ）を活かして、半径をわずかにずらして同心円を多重描画
            const int centerX = static_cast<int>(wave.position.x);
            const int centerY = static_cast<int>(wave.position.y);
            const int baseRadius = static_cast<int>(wave.currentRadius);
            const int halfThickness = static_cast<int>(wave.thickness * 0.5f);

            for (int r = baseRadius - halfThickness; r <= baseRadius + halfThickness; ++r) {
                if (r > 0) {
                    DrawCircle(centerX, centerY, r, RingColor, FALSE);
                }
            }
        }

        // -------------------------------------------------------------------------
        // 2. 豆腐破片（回転する角切り四角形）の描画
        // -------------------------------------------------------------------------
        for (const auto& debris : m_debrisParticles) {
            const int alpha = static_cast<int>(debris.color.a * 255.0f);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

            // 【改善点】破片の回転を反映するため、2D回転行列の三角関数を計算
            const float cosR = std::cos(debris.rotation);
            const float sinR = std::sin(debris.rotation);
            const float h = debris.size * 0.5f; // 半径（中心からのオフセット）

            // ローカル座標 (lx, ly) を回転させ、ワールド座標へオフセットするラムダ式
            auto rotatePoint = [&](float lx, float ly) {
                return std::pair<int, int>{
                    static_cast<int>(debris.position.x + (lx * cosR - ly * sinR)),
                        static_cast<int>(debris.position.y + (lx * sinR + ly * cosR))
                };
                };

            // 正方形の4つの頂点 (左上、右上、右下、左下) を回転
            auto [x1, y1] = rotatePoint(-h, -h);
            auto [x2, y2] = rotatePoint(h, -h);
            auto [x3, y3] = rotatePoint(h, h);
            auto [x4, y4] = rotatePoint(-h, h);

            // 塗りつぶしの豆腐本体を描画
            DrawQuadrangle(x1, y1, x2, y2, x3, y3, x4, y4, DebrisColor, TRUE);

            // 輪郭線を描いて豆腐の角（エッジ）を視覚的に際立たせる
            DrawQuadrangle(x1, y1, x2, y2, x3, y3, x4, y4, DebrisOutline, FALSE);
        }

        // 次の描画処理にアルファ合成が影響しないよう、ブレンドモードを通常描画にリセット
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    void ClickEffect::clear() noexcept {
        // シーン遷移時やリセット時に全要素を即座に破棄
        m_debrisParticles.clear();
        m_shockwaves.clear();
    }

} // namespace TofuGame