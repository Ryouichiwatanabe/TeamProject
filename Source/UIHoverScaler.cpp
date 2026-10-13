#include "UIHoverScaler.h"

UIHoverScaler::UIHoverScaler()
    : mNormalScale(1.0f)
    , mHoverScale(1.08f)
    , mPressScale(0.94f)
    , mLerpSpeed(18.0f)
    , mCurrentScale(1.0f)
{
}

void UIHoverScaler::Initialize(float normalScale, float hoverScale, float pressScale, float lerpSpeed)
{
    mNormalScale = normalScale;
    mHoverScale = hoverScale;
    mPressScale = pressScale;
    mLerpSpeed = lerpSpeed;
    mCurrentScale = normalScale;
}

void UIHoverScaler::Update(bool isHovered, bool isPressed, float deltaTime)
{
    // 状態に応じた目標スケールを決定
    float targetScale = mNormalScale;

    if (isHovered)
    {
        // ホバー中でクリック中なら「縮小（へこむ）」、ホバーのみなら「拡大」
        targetScale = isPressed ? mPressScale : mHoverScale;
    }

    // 線形補間（Lerp）で目標スケールへ滑らかに遷移
    mCurrentScale += (targetScale - mCurrentScale) * (mLerpSpeed * deltaTime);
}

void UIHoverScaler::CalculateRect(int baseLeft, int baseTop, int baseWidth, int baseHeight,
    int* outLeft, int* outTop, int* outRight, int* outBottom) const
{
    // 中心を基準にして外側・内側へ伸縮
    float halfWidth = (baseWidth * mCurrentScale) * 0.5f;
    float halfHeight = (baseHeight * mCurrentScale) * 0.5f;

    float centerX = baseLeft + (baseWidth * 0.5f);
    float centerY = baseTop + (baseHeight * 0.5f);

    if (outLeft)   *outLeft = static_cast<int>(centerX - halfWidth);
    if (outTop)    *outTop = static_cast<int>(centerY - halfHeight);
    if (outRight)  *outRight = static_cast<int>(centerX + halfWidth);
    if (outBottom) *outBottom = static_cast<int>(centerY + halfHeight);
}