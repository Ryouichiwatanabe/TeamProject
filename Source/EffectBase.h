#pragma once
#include <algorithm>
#include "DxLib.h"

class EffectBase
{
public:
  
    /* *  コンストラクタ
     *  fX 初期X座標
     *  fY 初期Y座標
     *  fLifeTime エフェクトの生存時間（秒）。0以下の場合は時間経過では自動消滅しない*/
     
    EffectBase(float fX, float fY, float fLifeTime = 1.0f)
        : m_fX(fX)
        , m_fY(fY)
        , m_fLifeTime(fLifeTime)
    {
    }

    virtual ~EffectBase() = default;

    // コピー・ムーブ禁止
    EffectBase(const EffectBase&) = delete;
    EffectBase(EffectBase&&) = default;

    /**
     * 外部（EffectManagerなど）から呼ばれる更新関数
     */
    void Update(float fDeltaTime)
    {
        if (m_bIsDead) return;

        m_fElapsedTime += fDeltaTime;

        // 寿命が設定されている場合、時間を超えたら自動で死亡フラグを立てる
        if (m_fLifeTime > 0.0f && m_fElapsedTime >= m_fLifeTime)
        {
            m_bIsDead = true;
        }

        // 派生クラス固有の更新処理を呼び出す
        OnUpdate(fDeltaTime);
    }

    /**
     * 描画処理（派生クラスで実装）
     */
    virtual void Draw() const = 0;

    // --- 状態取得・操作インターフェース --- //

    [[nodiscard]] bool IsDead() const noexcept { return m_bIsDead; }

    /// 即座に消滅させたい場合に呼び出す
    void Kill() noexcept { m_bIsDead = true; }

    /// 進行度（0.0f: 開始時 ～ 1.0f: 終了時）を取得。イージングやフェードアウトの計算に便利
    [[nodiscard]] float GetProgress() const noexcept
    {
        if (m_fLifeTime <= 0.0f) return 0.0f;
        return std::clamp(m_fElapsedTime / m_fLifeTime, 0.0f, 1.0f);
    }

    [[nodiscard]] float GetX() const noexcept { return m_fX; }
    [[nodiscard]] float GetY() const noexcept { return m_fY; }

protected:
    /**
     *  派生クラス側で固有の更新処理を記述する（任意でオーバーライド）
     */
    virtual void OnUpdate([[maybe_unused]] float fDeltaTime) {}

    float m_fX = 0.0f;
    float m_fY = 0.0f;
    float m_fElapsedTime = 0.0f; // 経過時間（秒）
    float m_fLifeTime = 1.0f;    // 最大生存時間（秒）
    bool  m_bIsDead = false;
};