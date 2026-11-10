#pragma once
#include "EffectBase.h"
#include "EffectSteamConfig.h"
#include <vector>

// ステージ背景の鍋から立ち上る湯気エフェクト
class EffectSteam final : public EffectBase
{
public:
    static constexpr int DEFAULT_STAGE_NUMBER = 1;

    // ステージ番号を指定して生成（EffectSteamConfigの定数を自動で読み取る）
    explicit EffectSteam(int _stageNumber = DEFAULT_STAGE_NUMBER);

    // 座標と範囲を直接指定して生成することも可能
    EffectSteam(
        float fCenterX,
        float fCenterY,
        float fSpawnRadiusX,
        float fSpawnMinY = EffectSteamConfig::STAGE_1.fSpawnMinY,
        float fSpawnMaxY = EffectSteamConfig::STAGE_1.fSpawnMaxY,
        int   nWispCount = EffectSteamConfig::STAGE_1.nWispCount
    );

    ~EffectSteam() override = default;

    void Draw() const override;

    bool IsBackgroundEffect() const noexcept override { return true; }

protected:
    void OnUpdate(float fDeltaTime) override;

private:
    struct SteamWisp
    {
        float fOriginX = 0.0f; // 湧き出し元のX座標
        float fCurrentY = 0.0f; // 現在の基準Y座標
        float fRiseSpeed = 0.0f; // 上昇スピード
        float fSwayAmp = 0.0f; // 左右のゆらぎ幅
        float fSwayFreq = 0.0f; // ゆらぎの周期
        float fPhase = 0.0f; // ゆらぎの初期位相
        float fBaseWidth = 0.0f; // 湯気の太さ
        float fAge = 0.0f; // 経過時間
        float fMaxLife = 0.0f; // 寿命
    };

    void InitializeWisps(int nWispCount);
    void ResetWisp(SteamWisp& wisp, bool bRandomInitialAge);

private:
    std::vector<SteamWisp> m_wisps;

    // ステージごとにEffectSteamConfigから読み込まれる範囲パラメータ
    float m_fSpawnRadiusX = 0.0f;
    float m_fSpawnMinY = 0.0f;
    float m_fSpawnMaxY = 0.0f;

    // --- 共通定数（マジックナンバー排除） ---
    static constexpr float HALF_RATIO = 0.5f;
    static constexpr float INFINITE_LIFE_TIME = 0.0f;   // EffectBaseの無期限再生用
    static constexpr float INITIAL_AGE_ZERO = 0.0f;

    // --- ランダム範囲パラメータ ---
    static constexpr float MIN_RISE_SPEED = 55.0f;  // 最小上昇速度
    static constexpr float MAX_RISE_SPEED = 95.0f;  // 最大上昇速度
    static constexpr float MIN_SWAY_AMP = 18.0f;  // 最小ゆらぎ幅
    static constexpr float MAX_SWAY_AMP = 42.0f;  // 最大ゆらぎ幅
    static constexpr float MIN_SWAY_FREQ = 1.6f;   // 最小ゆらぎ速度
    static constexpr float MAX_SWAY_FREQ = 3.0f;   // 最大ゆらぎ速度
    static constexpr float MIN_PHASE = 0.0f;   // 最小位相
    static constexpr float MAX_PHASE = 6.283185f; // 最大位相(2π)
    static constexpr float MIN_BASE_WIDTH = 20.0f;  // 湯気の最小太さ
    static constexpr float MAX_BASE_WIDTH = 38.0f;  // 湯気の最大太さ
    static constexpr float MIN_LIFE_TIME = 3.2f;   // 最短寿命
    static constexpr float MAX_LIFE_TIME = 5.0f;   // 最長寿命

    // --- 湯気の見た目・アニメーション定数 ---
    static constexpr int   WISP_NODE_COUNT = 6;      // 1本の湯気の筋を構成する縦の連なり数
    static constexpr float NODE_STEP_Y = 22.0f;  // 連なり1つあたりの縦間隔
    static constexpr float NODE_PHASE_DELAY = 0.45f;  // 上に行くほどS字に遅れてくねる位相差
    static constexpr float NODE_TAPER_RATE = 0.12f;  // 上下の端を細くする係数
    static constexpr float EXPAND_SCALE_RATE = 1.3f;   // 上昇するにつれてふわっと広がる倍率
    static constexpr float VERTICAL_STRETCH = 1.65f;  // 湯気らしく縦長に引き伸ばす比率

    // --- 透明度・色定数 ---
    static constexpr float FADE_IN_END_RATIO = 0.25f;  // ふわっと現れるまでの進行割合
    static constexpr float FADE_OUT_SPAN_RATIO = 0.75f;  // 消えていく区間の割合 (1.0 - 0.25)
    static constexpr float UNIT_RATIO = 1.0f;
    static constexpr float MAX_OUTER_ALPHA = 28.0f;  // 外側のふんわりした湯気の最大アルファ
    static constexpr float MAX_CORE_ALPHA = 22.0f;  // 内側の湯気の芯の最大アルファ
    static constexpr float OUTER_RADIUS_RATE = 1.45f;  // 外側の霞の広がり倍率
    static constexpr int   MIN_VISIBLE_ALPHA = 2;      // 描画をスキップする最小アルファ閾値
    static constexpr int   NOBLEND_PARAM = 0;      // ブレンド解除用

    static constexpr int   STEAM_COLOR_R = 252;    // 温かい湯気の白（R）
    static constexpr int   STEAM_COLOR_G = 250;    // 温かい湯気の白（G）
    static constexpr int   STEAM_COLOR_B = 245;    // 温かい湯気の白（B）
};