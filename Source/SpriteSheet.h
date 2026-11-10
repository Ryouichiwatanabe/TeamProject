#pragma once
#include "EffectBase.h"
#include <algorithm>

struct DivGraphData;

// 分割画像（スプライトシート）をコマ送り再生するクラス
class SpriteSheet : public EffectBase
{
public:
    // --- 定数定義 ---
    static constexpr int    MIN_COLOR_VALUE = 0;     // 色・透明度の最小値
    static constexpr int    MAX_COLOR_VALUE = 255;   // 色・透明度の最大値
    static constexpr int    INVALID_HANDLE = -1;    // 無効な画像ハンドル
    static constexpr float  CENTER_PIVOT = -1.0f; // 画像中央を基準にする指定値
    static constexpr float  INFINITE_LIFE = 0.0f;  // ループ再生用（寿命なし）
    static constexpr double DEFAULT_SCALE = 1.0;   // 基本の拡大率

public:
    // コンストラクタ（座標・画像・1コマ秒数・ループ有無・拡大率）
    SpriteSheet(
        float fX,
        float fY,
        const DivGraphData* pData,
        float fFrameDuration,
        bool bIsLoop = false,
        double dScale = DEFAULT_SCALE
    );

    ~SpriteSheet() override = default;

    // 現在のコマを描画
    void Draw() const override;

    // --- 見た目調整用のSetter ---
    void SetScale(double dScale) { m_dScale = dScale; }         // 拡大率の設定
    void SetAngle(double dAngle) { m_dAngle = dAngle; }         // 回転角の設定
    void SetAdditiveBlend(bool bIsAdd) { m_bIsAddBlend = bIsAdd; }    // 加算合成のON/OFF
    void SetPivot(float fX, float fY) { m_fPivotX = fX; m_fPivotY = fY; } // 描画中心の設定
    void SetAlpha(int nAlpha) { m_nAlpha = std::clamp(nAlpha, MIN_COLOR_VALUE, MAX_COLOR_VALUE); } // 透明度の設定
    void SetColor(int r, int g, int b) // 色味（RGB）の設定
    {
        m_nRed = std::clamp(r, MIN_COLOR_VALUE, MAX_COLOR_VALUE);
        m_nGreen = std::clamp(g, MIN_COLOR_VALUE, MAX_COLOR_VALUE);
        m_nBlue = std::clamp(b, MIN_COLOR_VALUE, MAX_COLOR_VALUE);
    }

private:
    int  GetCurrentFrameIndex() const; // 今何コマ目かを返す
    bool HasCustomColor() const;       // 色変更があるか判定
    bool HasCustomPivot() const;       // 中心ズレ指定があるか判定

private:
    const DivGraphData* m_pDivData = nullptr; // 分割画像データ
    float  m_fFrameDuration = 0.1f;           // 1コマの表示秒数
    bool   m_bIsLoop = false;          // ループ再生フラグ
    bool   m_bIsAddBlend = false;          // 加算合成フラグ

    double m_dScale = DEFAULT_SCALE;  // 拡大率
    double m_dAngle = 0.0;            // 回転角度

    int    m_nAlpha = MAX_COLOR_VALUE;// 透明度
    int    m_nRed = MAX_COLOR_VALUE;// 赤成分
    int    m_nGreen = MAX_COLOR_VALUE;// 緑成分
    int    m_nBlue = MAX_COLOR_VALUE;// 青成分

    float  m_fPivotX = CENTER_PIVOT;   // 中心X（負なら中央）
    float  m_fPivotY = CENTER_PIVOT;   // 中心Y（負なら中央）
};