#include "EffectClear.h"
#include "Master.h"
#include "ResourceManager.h"
#include <cmath>

EffectClear::EffectClear(float fCenterX, float fCenterY)
    : EffectBase(fCenterX, fCenterY, DURATION)
{
    // =================================================================
    // 1. テクスチャの読み込みとサイズ取得
    // =================================================================
    m_nGraphHandle = Master::mpResource->LoadGraphics(std::string(TEXTURE_PATH));

    if (m_nGraphHandle != -1)
    {
        int sizeX = 0, sizeY = 0;
        GetGraphSize(m_nGraphHandle, &sizeX, &sizeY);
        m_fImageWidth = static_cast<float>(sizeX);
        m_fImageHeight = static_cast<float>(sizeY);
    }

    // メモリ再確保を防ぐため、全パーティクル分の領域をあらかじめ確保
    m_sparks.reserve(OUTER_COUNT + INNER_COUNT);

    // =================================================================
    // 2. 花火カラーパレットの定義
    // 【調整メモ】
    // 色の組み合わせを変えることで「お祝い感」「クール」「ファンシー」等に雰囲気を変更可能
    // =================================================================
    const COLOR_U8 palette[] = {
        GetColorU8(255, 220,  80, 255), // ゴールド
        GetColorU8(255,  80, 160, 255), // ビビッドピンク
        GetColorU8(80, 240, 255, 255), // シアン
        GetColorU8(255, 255, 255, 255), // 純白
        GetColorU8(255, 140,  30, 255), // オレンジ
        GetColorU8(180, 100, 255, 255)  // パープル
    };
    constexpr int PALETTE_SIZE = static_cast<int>(sizeof(palette) / sizeof(palette[0]));

    // 乱数ヘルパー: 指定した範囲 [min, max] の小数を生成
    auto getRandFloat = [](float min, float max) {
        return min + (static_cast<float>(GetRand(10000)) / 10000.0f) * (max - min);
        };

    // =================================================================
    // 3. 火花生成ラムダ式
    // =================================================================
    auto createSparks = [&](int count, float minSpeed, float maxSpeed, float scaleMin, float scaleMax) {
        for (int i = 0; i < count; ++i)
        {
            // 均等分割ではなく完全ランダムな角度（0〜360度）に飛ばして自然な塊・バラつきを作る
            const float angle = getRandFloat(0.0f, DX_TWO_PI_F);

            // 初速に強弱をつける（遠くまで飛ぶ粒と手前に残る粒の差を作る）
            const float speed = getRandFloat(minSpeed, maxSpeed);

            RingSpark spark;

            // 発生位置を中心からわずかにズラす（完全な幾何学的一点から出る不自然さを解消）
            // 【調整メモ】25.0f を大きくすると最初から広がった状態で破裂する
            const float startOffset = getRandFloat(0.0f, 25.0f);
            spark.fX = m_fX + std::cos(angle) * startOffset;
            spark.fY = m_fY + std::sin(angle) * startOffset;

            // 角度と速度から X, Y 方向の速度ベクトルを算出
            spark.fVx = std::cos(angle) * speed;
            spark.fVy = std::sin(angle) * speed;

            // パーティクルの初期パラメータ設定
            spark.fScale = getRandFloat(scaleMin, scaleMax);
            spark.fRotation = getRandFloat(0.0f, DX_TWO_PI_F);
            // 回転速度: 正負両方に振ることで時計回りと反時計回りが混ざる
            spark.fRotSpeed = getRandFloat(-6.0f, 6.0f);
            // 点滅の開始タイミングをバラけさせるオフセット
            spark.fFlickerOffset = getRandFloat(0.0f, 10.0f);
            // パレットからランダムに色を選択
            spark.color = palette[GetRand(PALETTE_SIZE - 1)];

            m_sparks.emplace_back(spark);
        }
        };

    // =================================================================
    // 4. 火花生成の実行（外側と内側で速度と大きさを分担）
    // =================================================================

    // ① 主力弾頭：画面外・画面端まで突き抜ける超高速火花
    // 【調整メモ】
    // ・速度 (350〜950): 上限を上げると画面全体へさらに飛び散る
    // ・サイズ (0.9〜1.8): 上限を上げると大粒になり迫力が増す
    createSparks(OUTER_COUNT, 350.0f, 950.0f, 0.9f, 2.2f);

    // ② 中間層：中心〜中間空間を埋めてスカスカ感をなくす中速火花
    // 【調整メモ】
    // ・速度 (120〜500): 下限を下げると爆心地近くに火花が滞留する
    createSparks(INNER_COUNT, 120.0f, 500.0f, 0.5f, 1.0f);
}

void EffectClear::OnUpdate(float fDeltaTime)
{
    // =================================================================
    // 物理挙動パラメータ
    // =================================================================

    // 下方向への重力加速度 (px/s^2)
    // 【調整メモ】
    // ・大きくする (例: 300.0f) -> 本物の花火のように後半グッと下に垂れ下がる
    // ・小さくする (例: 0.0f)   -> 重力の影響を受けず四方八方へ均等に飛び続ける（無重力感）
    constexpr float GRAVITY = 200.0f;

    // 空気抵抗係数（1フレーム 60fps 基準）
    // 【調整メモ】
    // ・小さくする (例: 0.90f) -> 強力なブレーキがかかり、弾けたあとすぐ失速して止まる
    // ・大きくする (例: 0.98f) -> 失速せず、初速の勢いのまま画面外まで伸びていく
    constexpr float AIR_RESIST = 0.99f;

    for (auto& spark : m_sparks)
    {
        // 過去座標を先頭に追加（トレイル用）
        spark.trailPositions.push_front({ spark.fX, spark.fY });

        // トレイルの最大保持フレーム数
        // 【調整メモ】
        // ・増やす (例: 12) -> 光の尾が長く伸びて流星のようなスピード感が出る
        // ・減らす (例: 4)  -> 尾が短くなり、歯切れの良い粒感になる
        if (spark.trailPositions.size() > 3)
        {
            spark.trailPositions.pop_back();
        }

        // 速度ベクトルを加算して座標を更新
        spark.fX += spark.fVx * fDeltaTime;
        spark.fY += spark.fVy * fDeltaTime;

        // Y軸速度に重力を加算
        spark.fVy += GRAVITY * fDeltaTime;

        // 可変フレームレート（fDeltaTime）に対応した空気抵抗の減速計算
        const float resist = std::pow(AIR_RESIST, fDeltaTime * 60.0f);
        spark.fVx *= resist;
        spark.fVy *= resist;

        // パーティクル自体の回転角更新
        spark.fRotation += spark.fRotSpeed * fDeltaTime;
    }
}

void EffectClear::Draw() const
{
    // 全体進行度 (0.0f: 開始直後 〜 1.0f: 終了)
    const float fProgress = GetProgress();

    // =================================================================
    // 1. 開幕の画面フラッシュ演出（爆心地の閃光・衝撃波）
    // 【調整メモ】
    // ・判定時間 (0.15f): 閃光が見えている時間。0.1fにすると瞬発力重視、0.3fにすると余韻重視
    // =================================================================
    if (fProgress < 0.15f)
    {
        // 1.0f から 0.0f へ直線的に減衰するフェードアウト比率
        const float flashRatio = 1.0f - (fProgress / 0.15f);

        // 中心部を塗りつぶし円で発光（爆発コア）
        // 【調整メモ】120.0f を大きくすると爆心地の光の玉が大きくなる
        SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(flashRatio * 255.0f));
        DrawCircle(
            static_cast<int>(m_fX),
            static_cast<int>(m_fY),
            static_cast<int>(120.0f * (1.0f - flashRatio)),
            GetColor(255, 255, 230),
            TRUE // TRUE = 塗りつぶし
        );

        // 急速に広がる外枠ショックウェーブリング
        // 【調整メモ】700.0f を広げると画面端まで衝撃波が走る
        SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(flashRatio * 180.0f));
        DrawCircle(
            static_cast<int>(m_fX),
            static_cast<int>(m_fY),
            static_cast<int>(700.0f * (fProgress / 0.15f)),
            GetColor(255, 220, 160),
            FALSE // FALSE = 枠線のみ
        );
    }

    // 画像がロードできていなければここで終了
    if (m_nGraphHandle == -1)
    {
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        return;
    }

    // 後半に向かって急激に消えるアルファ減衰カーブ（燃え尽きる質感）
    // 【調整メモ】累乗数（1.5f）を 2.0f にすると消え際がより鋭くなり、1.0f にすると等速フェードになる
    const float alphaCurve = 1.0f - std::pow(fProgress, 1.5f);
    const float baseAlpha = 255.0f * alphaCurve;
    const float fCenterX = m_fImageWidth / 2.0f;
    const float fCenterY = m_fImageHeight / 2.0f;

    for (const auto& spark : m_sparks)
    {
        // 高速サイン波によるチカチカした明滅演出
        // 【調整メモ】
        // ・45.0f を大きくすると点滅が激しくなり、小さくするとフワフワした明滅になる
        const float flicker = 0.7f + 0.3f * std::sin((m_fElapsedTime * 45.0f) + spark.fFlickerOffset);
        const int currentAlpha = static_cast<int>(baseAlpha * flicker);

        if (currentAlpha <= 0) continue;

        // パーティクルの個別カラーをテクスチャに乗算着色
        SetDrawBright(spark.color.r, spark.color.g, spark.color.b);

        // =============================================================
        // 2. 残像（トレイル）の描画
        // =============================================================
        float trailScaleFactor = 0.75f; // 過去のフレームほど小さく描画
        float trailAlphaFactor = 0.45f; // 過去のフレームほど薄く描画

        for (const auto& [tx, ty] : spark.trailPositions)
        {
            SetDrawBlendMode(DX_BLENDMODE_ADD, static_cast<int>(currentAlpha * trailAlphaFactor));

            DrawRotaGraph3(
                static_cast<int>(tx),
                static_cast<int>(ty),
                static_cast<int>(fCenterX),
                static_cast<int>(fCenterY),
                spark.fScale * trailScaleFactor,
                spark.fScale * trailScaleFactor,
                static_cast<double>(spark.fRotation),
                m_nGraphHandle,
                TRUE,
                FALSE
            );

            // 過去ログを遡るごとにさらに縮小・透過させる
            trailScaleFactor *= 0.75f;
            trailAlphaFactor *= 0.5f;
        }

        // =============================================================
        // 3. パーティクル本体の描画
        // =============================================================
        // 時間経過とともにわずかに縮小（燃え尽きる火花を表現）
        // 【調整メモ】0.4f を 0.8f などに上げると、消える直前に豆粒のように小さくなる
        const float currentScale = spark.fScale * (1.0f - fProgress * 0.4f);

        SetDrawBlendMode(DX_BLENDMODE_ADD, currentAlpha);

        DrawRotaGraph3(
            static_cast<int>(spark.fX),
            static_cast<int>(spark.fY),
            static_cast<int>(fCenterX),
            static_cast<int>(fCenterY),
            currentScale,
            currentScale,
            static_cast<double>(spark.fRotation),
            m_nGraphHandle,
            TRUE,
            FALSE
        );
    }

    // =================================================================
    // 描画ステートのリセット（他描画への影響を防止）
    // =================================================================
    SetDrawBright(255, 255, 255);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}