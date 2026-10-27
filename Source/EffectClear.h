#pragma once

#include <vector>
#include <deque>
#include <string_view>
#include "EffectBase.h"

// ステージクリア時の花火演出クラス
class EffectClear final : public EffectBase
{
public:
    // コンストラクタ（発生位置の中心座標を受け取る）
    EffectClear(float fCenterX, float fCenterY);

    // デストラクタ
    ~EffectClear() override = default;

    // 描画（開幕の閃光、光の尾、パーティクル本体）
    void Draw() const override;

protected:
    // 物理更新（空気抵抗による減速、重力、トレイル座標の記録）
    void OnUpdate(float fDeltaTime) override;

private:
    // =================================================================
    // 個々の火花パーティクル情報
    // =================================================================
    struct RingSpark
    {
        float fX = 0.0f;               // 現在のX座標
        float fY = 0.0f;               // 現在のY座標
        float fVx = 0.0f;              // X軸の移動速度 (px/s)
        float fVy = 0.0f;              // Y軸の移動速度 (px/s)
        float fScale = 1.0f;           // 描画サイズ倍率
        float fRotation = 0.0f;        // 画像の回転角度（ラジアン）
        float fRotSpeed = 0.0f;        // 回転速度（1秒あたりの回転量）
        float fFlickerOffset = 0.0f;   // チカチカ明滅のタイミング調整用（一斉に点滅するのを防ぐ）
        COLOR_U8 color{};              // パレットから選ばれたパーティクルの色 (RGB)

        // 過去数フレームの座標履歴（残像・光の尾を描くために使用）
        // ※ OnUpdate内の保持数を増やすと尾が長く伸び、減らすと短くなります
        std::deque<std::pair<float, float>> trailPositions;
    };

    // =================================================================
    // 調整用パラメータ（ここを変えると見た目のスケールや密度が変わります）
    // =================================================================

    // 読み込むテクスチャ画像のパス（白単色の星型や光球を推奨）
    static constexpr std::string_view TEXTURE_PATH = "Resource/Clear.png";

    // エフェクトが消えるまでの総時間（秒）
    // 【調整メモ】
    // ・数値を大きくする (例: 2.2f) -> フワッと長く余韻を残して漂う
    // ・数値を小さくする (例: 1.0f) -> 一瞬で激しく弾けて終わるキレ重視になる
    static constexpr float DURATION = 2.6f;

    // 外側へ高速に飛び散る大粒の数
    // 【調整メモ】
    // ・増やす (例: 70) -> 画面全体を覆うような密度の高い大爆発になる
    // ・減らす (例: 24) -> 粒がまばらになり、すっきりした印象になる
    static constexpr int OUTER_COUNT = 48;

    // 内側に残りながら広がる中〜小粒の数
    // 【調整メモ】
    // ・増やす (例: 50) -> 中心付近がキラキラと埋まり、空間のスカスカ感が消える
    // ・減らす (例: 16) -> 外側へ抜けるリング状の広がりが強調される
    static constexpr int INNER_COUNT = 32;

    // =================================================================
    // 内部メンバ変数
    // =================================================================

    int   m_nGraphHandle = -1;  // 画像のグラフィックハンドル
    float m_fImageWidth = 0.0f;  // 画像の横幅（回転の中心点計算に使用）
    float m_fImageHeight = 0.0f;  // 画像の縦幅（回転の中心点計算に使用）

    // 生成した火花の配列（合計: OUTER_COUNT + INNER_COUNT 個を保持）
    std::vector<RingSpark> m_sparks;
};