#include "UIFloatingMotion.h"
#include <algorithm>
#include <cmath>

void UIFloatingMotion::SetParameter(int _periodFrames, float _amplitude)
{
    mnPeriod = (std::max)(1, _periodFrames);   // 0割り防止
    mfAmplitude = _amplitude;
}

void UIFloatingMotion::Update()
{
    if (!mbActive) return;

    // Todo : deltaTime対応(HoverScalerと同じタイミングで)
    mnFrame = (mnFrame + 1) % mnPeriod;
}

VECTOR UIFloatingMotion::GetOffset() const
{
    if (!mbActive) return VGet(0.0f, 0.0f, 0.0f);

    const float progress = static_cast<float>(mnFrame) / mnPeriod;
    return VGet(0.0f, std::sin(progress * DX_TWO_PI_F) * mfAmplitude, 0.0f);
}