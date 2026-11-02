#pragma once
#include "DxLib.h"
#include "ScreenConfig.h"

// 画面遷移や演出用のフェード処理を管理・描画するクラス
class FadeEffect {
public:
    enum class State {
        Idle,     // 停止中（IsCovered() で真っ黒か透明かを判別）
        FadeIn,   // 黒 → 画面（アルファ 255 → 0）
        FadeOut   // 画面 → 黒（アルファ 0 → 255）
    };

    explicit FadeEffect(int defaultDurationFrames = 60)
        : mnDefaultDuration(defaultDurationFrames > 0 ? defaultDurationFrames : 1)
        , mnDurationFrames(mnDefaultDuration)
    {}

    // frames <= 0 ならデフォルト時間を使う（デフォルト値は書き換えない）
    void StartFadeIn(int frames = -1) { StartFade(State::FadeIn, frames); }
    void StartFadeOut(int frames = -1) { StartFade(State::FadeOut, frames); }

    // このフレームでフェードが完了したら true を返す
    [[nodiscard]] bool Update() {
        if (meState == State::Idle) return false;

        if (++mnCurrentFrames >= mnDurationFrames) {
            mnCurrentFrames = mnDurationFrames;
            mbCovered = (meState == State::FadeOut);  // 暗転で終わったか覚えておく
            meState = State::Idle;
            return true;
        }
        return false;
    }

    void Draw(unsigned int color = 0) const {
        const int alpha = GetAlpha();
        if (alpha <= 0) return;

        int prevMode = 0, prevParam = 0;
        GetDrawBlendMode(&prevMode, &prevParam);   // 呼び出し元の設定を保存

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
        DrawBox(0, 0,
            static_cast<int>(ScreenConfig::SCREEN_WIDTH),
            static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
            color, TRUE);

        SetDrawBlendMode(prevMode, prevParam);      // 元に戻す
    }

    int GetAlpha() const {
        const float p = GetProgress();
        switch (meState) {
        case State::FadeIn:  return static_cast<int>((1.0f - p) * 255.0f);
        case State::FadeOut: return static_cast<int>(p * 255.0f);
        case State::Idle:
        default:             return mbCovered ? 255 : 0;
        }
    }

    float GetProgress() const {
        return static_cast<float>(mnCurrentFrames) / static_cast<float>(mnDurationFrames);
    }

    bool  IsFinished() const { return meState == State::Idle; }
    bool  IsCovered()  const { return meState == State::Idle && mbCovered; }
    State GetState()   const { return meState; }

private:
    void StartFade(State next, int frames) {
        mnDurationFrames = (frames > 0) ? frames : mnDefaultDuration;
        mnCurrentFrames = 0;
        meState = next;
    }

    int   mnDefaultDuration;          // 引数省略時のフェード時間
    int   mnDurationFrames;           // 今回のフェード時間
    int   mnCurrentFrames = 0;        // 経過フレーム数
    State meState = State::Idle;
    bool  mbCovered = false;    // Idle 時に画面が黒で覆われているか
};