#pragma once
#include <vector>

// 1つのパーティクル（紙吹雪 / 光の粒）
struct Particle {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;          // X方向の速度
    float vy = 0.0f;          // Y方向の速度
    float size = 0.0f;        // サイズ
    float angle = 0.0f;       // 回転角度
    float rotSpeed = 0.0f;    // 回転速度
    unsigned int color = 0;            // 色
    int life = 0;             // 残り生存フレーム数
    int maxLife = 0;          // 初期生存フレーム数
    int type = 0;             // 0: 紙吹雪, 1: 光の粒
};

// 衝撃波リング
struct ShockWave {
    float x = 0.0f;
    float y = 0.0f;
    float radius = 0.0f;      // 現在の半径
    float speed = 0.0f;       // 広がる速度
    int life = 0;
    int maxLife = 0;
};

// ゴールエフェクト管理クラス
class GoalEffect {
public:
    GoalEffect() = default;

    // 指定した座標でエフェクトを発生させる
    void Trigger(float startX, float startY);

    // 毎フレームの更新処理
    void Update();

    // 毎フレームの描画処理
    void Draw() const;

    // すべてのエフェクトをリセット
    void Clear();

private:
    std::vector<Particle> m_particles;
    std::vector<ShockWave> m_waves;
};