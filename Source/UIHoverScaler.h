#pragma once

class UIHoverScaler
{
public:
    UIHoverScaler();

    /// <summary>
    /// スケールパラメータの初期化
    /// </summary>
    /// <param name="normalScale">通常時の倍率（基本: 1.0f）</param>
    /// <param name="hoverScale">カーソルが乗ったときの拡大率（例: 1.08f）</param>
    /// <param name="pressScale">クリック押し込み時の縮小率（例: 0.94f）</param>
    /// <param name="lerpSpeed">追従速度（15.0f?20.0f程度で自然な手触り）</param>
    void Initialize(float normalScale = 1.0f, float hoverScale = 1.08f, float pressScale = 0.94f, float lerpSpeed = 18.0f);

    /// <summary>
    /// 毎フレームの拡縮補間計算
    /// </summary>
    /// <param name="isHovered">マウスがUIの上にあるか</param>
    /// <param name="isPressed">マウスボタンが押されているか</param>
    /// <param name="deltaTime">前フレームからの経過時間</param>
    void Update(bool isHovered, bool isPressed, float deltaTime);

    /// <summary>
    /// 基準となる矩形座標を中心を軸に拡大・縮小した座標を算出
    /// </summary>
    void CalculateRect(int baseLeft, int baseTop, int baseWidth, int baseHeight,
        int* outLeft, int* outTop, int* outRight, int* outBottom) const;

    /// <summary>
    /// 現在の拡大率を取得
    /// </summary>
    float GetCurrentScale() const { return mCurrentScale; }

private:
    float mNormalScale;
    float mHoverScale;
    float mPressScale;
    float mLerpSpeed;

    float mCurrentScale;
};