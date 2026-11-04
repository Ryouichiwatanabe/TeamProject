#pragma once

class UI;

class UIHoverScaler
{
public:
    UIHoverScaler() = default;
    ~UIHoverScaler() = default;

    void Initialize(UI* _pUI) { mpUI = _pUI; }   // 他のモジュールと同じ形

    void SetActive(bool _active) { mbActive = _active; }
    bool IsActive() const { return mbActive; }

    void Update();                                // 引数なし
    float GetScale() const { return mfCurrentScale; }

private:
    UI* mpUI = nullptr;

    bool  mbActive = false;   // 既定は無効(オプトイン)
    float mfHoverScale = 1.08f;   // ホバー中の目標倍率
    float mfLerpRate = 0.3f;    // 1フレームで目標へ近づく割合(60fps想定)
    float mfCurrentScale = 1.0f;
};