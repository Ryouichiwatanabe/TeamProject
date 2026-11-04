#pragma once
#include "FadeEffect.h"

// シーン遷移の演出手順（暗転 → 差し替え → 明転）を管理するクラス
class SceneTransition {
public:
    explicit SceneTransition(int frames = 30) : mFade(frames) {}

    void Start() { mePhase = Phase::Out; mFade.StartFadeOut(); }
    void StartFromBlack() { mePhase = Phase::In;  mFade.StartFadeIn(); }  // 起動時用

    // シーンを差し替えるべきフレームで true を返す
    [[nodiscard]] bool Update() {
        if (!mFade.Update()) return false;      // このフレームでフェードが終わっていない

        if (mePhase == Phase::Out) {            // 暗転が終わった
            mePhase = Phase::In;
            mFade.StartFadeIn();
            return true;
        }
        mePhase = Phase::None;                  // 明転が終わった → 遷移完了
        return false;
    }

    void Draw() const { mFade.Draw(); }
    bool IsActive() const { return mePhase != Phase::None; }

private:
    enum class Phase { None, Out, In };
    Phase      mePhase = Phase::None;
    FadeEffect mFade;
};