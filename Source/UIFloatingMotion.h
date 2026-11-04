// UIFloatingMotion.h
#pragma once
#include <DxLib.h>

class UI;

class UIFloatingMotion
{
public:
    UIFloatingMotion() = default;
    ~UIFloatingMotion() = default;

    void Initialize(UI* _pUI) { mpUI = _pUI; }   // 今は未使用。モジュールの形を揃えるため

    void SetActive(bool _active) { mbActive = _active; }
    bool IsActive() const { return mbActive; }

    // 周期(フレーム数)と振れ幅(px)。呼ばなければ既定値
    void SetParameter(int _periodFrames, float _amplitude);

    void Update();
    VECTOR GetOffset() const;   // 無効なら (0,0,0)

private:
    UI* mpUI = nullptr;

    bool  mbActive = false;   // 既定は無効(オプトイン)
    int   mnPeriod = 60;      // 1往復のフレーム数
    float mfAmplitude = 8.0f;    // 上下の振れ幅(px)
    int   mnFrame = 0;
};