#include "ClearUI.h"
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UIManager.h"
#include "ClearCollectibleUI.h"
#include "ScreenConfig.h"
#include "FontManager.h"

// =====================================================================
// 画面サイズ変換ヘルパー
// レイアウトは「1280x720の画面」として書き、描画時に実画面へ変換する
// =====================================================================
namespace
{
    // 元のレイアウトを作った基準解像度
    constexpr float BASE_WIDTH = 1280.0f;
    constexpr float BASE_HEIGHT = 720.0f;

    // 縦横比を保ったまま画面に収まる倍率（1920x1080なら1.5）
    constexpr float SCALE_X = ScreenConfig::SCREEN_WIDTH / BASE_WIDTH;
    constexpr float SCALE_Y = ScreenConfig::SCREEN_HEIGHT / BASE_HEIGHT;
    constexpr float UI_SCALE = (SCALE_X < SCALE_Y) ? SCALE_X : SCALE_Y;

    // 16:9以外の画面でも中央に来るようにするズレ量（16:9なら0）
    constexpr float OFFSET_X = (ScreenConfig::SCREEN_WIDTH - BASE_WIDTH * UI_SCALE) * 0.5f;
    constexpr float OFFSET_Y = (ScreenConfig::SCREEN_HEIGHT - BASE_HEIGHT * UI_SCALE) * 0.5f;

    // 基準座標 → 実画面座標
    constexpr float ToScreenX(float _x) { return OFFSET_X + _x * UI_SCALE; }
    constexpr float ToScreenY(float _y) { return OFFSET_Y + _y * UI_SCALE; }

    // 基準の長さ → 実画面の長さ（幅・高さ・拡大率に使う）
    constexpr float ToScreenLength(float _length) { return _length * UI_SCALE; }

    // 基準座標で DrawBox する
    void DrawBoxBase(
        float _x1, float _y1,
        float _x2, float _y2,
        unsigned int _color,
        int _fillFlag
    )
    {
        DrawBox(
            static_cast<int>(ToScreenX(_x1)),
            static_cast<int>(ToScreenY(_y1)),
            static_cast<int>(ToScreenX(_x2)),
            static_cast<int>(ToScreenY(_y2)),
            _color,
            _fillFlag
        );
    }

    // 基準座標の「中心X・上端Y」に、文字列を中央揃えで描画する
    void DrawStringCenterBase(
        float _centerX,
        float _topY,
        const char* _text,
        unsigned int _color,
        int _fontHandle
    )
    {
        const int textWidth = GetDrawStringWidthToHandle(
            _text,
            static_cast<int>(strlen(_text)),
            _fontHandle
        );

        DrawStringToHandle(
            static_cast<int>(ToScreenX(_centerX)) - textWidth / 2,
            static_cast<int>(ToScreenY(_topY)),
            _text,
            _color,
            _fontHandle
        );
    }

    // アイテムの基準レイアウト（元の 550 + i*90, 315）
    constexpr float ITEM_START_X = 550.0f;
    constexpr float ITEM_SPACING = 90.0f;
    constexpr float ITEM_Y = 315.0f;
    constexpr float ITEM_LABEL_Y = 365.0f;
    constexpr float ITEM_SCALE = 0.5f;

    // 戻るボタンの基準レイアウト（中心640,485 / 280x70 → 500,450〜780,520）
    constexpr float BUTTON_CENTER_X = 640.0f;
    constexpr float BUTTON_CENTER_Y = 485.0f;
    constexpr float BUTTON_WIDTH = 280.0f;
    constexpr float BUTTON_HEIGHT = 70.0f;
}


ClearUI::ClearUI(
    const std::string& _filename0,
    const std::string& _filename1,
    const std::string& _filename2
)
{
    mCollectibleFilename[0] = _filename0;
    mCollectibleFilename[1] = _filename1;
    mCollectibleFilename[2] = _filename2;
}

void ClearUI::Init()
{
    SetEnabled(false);

    auto* uiManager =
        Master::mpSceneManager
        ->GetCurrentScene()
        ->GetUIManager();

    // クリアアイテム
    for (int i = 0; i < 3; i++)
    {
        mpCollectibleUI[i] =
            uiManager->CreateUI<ClearCollectibleUI>();

        mpCollectibleUI[i]->SetGraph(mCollectibleFilename[i]);

        mpCollectibleUI[i]->GetTransform().SetAnchor(
            VGet(0.0f, 0.0f, 0.0f)
        );

        mpCollectibleUI[i]->GetTransform().SetPosition(
            VGet(
                ToScreenX(ITEM_START_X + i * ITEM_SPACING),
                ToScreenY(ITEM_Y),
                0.0f
            )
        );

        const float scale = ToScreenLength(ITEM_SCALE);
        mpCollectibleUI[i]->GetTransform().SetScale(
            VGet(scale, scale, 1.0f)
        );

        // ClearUIの子UIにする
        AddChild(mpCollectibleUI[i]);
        mpCollectibleUI[i]->SetParent(this);

        // 最初は非表示
        mpCollectibleUI[i]->SetEnabled(false);
    }

    // ステージ選択へ戻るボタン
    mpStageSelectUI = uiManager->CreateUI<UI>();

    mpStageSelectUI->SetLayer(2);

    mpStageSelectUI->GetTransform().SetAnchor(
        VGet(0.0f, 0.0f, 0.0f)
    );

    mpStageSelectUI->GetTransform().SetPosition(
        VGet(
            ToScreenX(BUTTON_CENTER_X),
            ToScreenY(BUTTON_CENTER_Y),
            0.0f
        )
    );

    // クリック判定の大きさも画面に合わせる
    mpStageSelectUI->GetTransform().SetSize(
        VGet(
            ToScreenLength(BUTTON_WIDTH),
            ToScreenLength(BUTTON_HEIGHT),
            0.0f
        )
    );

    mpStageSelectUI->GetText().SetText("ステージ選択へ戻る");
    mpStageSelectUI->GetText().SetColor(GetColor(255, 255, 255));

    // ClearUIの子UIにする
    AddChild(mpStageSelectUI);
    mpStageSelectUI->SetParent(this);

    mpStageSelectUI->SetEnabled(true);
}


void ClearUI::Update()
{
    // アイテム出現アニメーション用タイマー
    mfItemTimer += 1.0f / 60.0f;

    // アイテムの獲得状態を反映
    for (int i = 0; i < 3; i++)
    {
        if (mpCollectibleUI[i] == nullptr)
        {
            continue;
        }

        if (mbIsCollected[i])
        {
            mpCollectibleUI[i]->SetCollected(true);
        }
    }

    // ステージ選択へ戻る
    if (mpStageSelectUI != nullptr &&
        mpStageSelectUI->GetButton().IsClicked())
    {
        Master::mpSceneManager->RequestScene(
            SCENE_TYPE::STAGE_SELECT_SCENE
        );
    }
}


void ClearUI::SetCollected(
    int _index,
    bool _isCollected
)
{
    if (_index < 0 || _index >= 3)
    {
        return;
    }

    mbIsCollected[_index] = _isCollected;
}


void ClearUI::Draw()
{
    // ---------------------------------------------------------------
    // 暗転（ここだけは基準座標ではなく、実画面全体を覆う）
    // ---------------------------------------------------------------
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);

    DrawBox(
        0,
        0,
        static_cast<int>(ScreenConfig::SCREEN_WIDTH),
        static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
        GetColor(0, 0, 0),
        TRUE
    );

    // ---------------------------------------------------------------
    // クリアパネル（以下はすべて1280x720基準の座標）
    // ---------------------------------------------------------------

    // 外側
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawBoxBase(400, 180, 880, 600, GetColor(80, 65, 45), TRUE);

    // 内側
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210);
    DrawBoxBase(410, 190, 870, 590, GetColor(245, 230, 190), TRUE);

    // 中央部分
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
    DrawBoxBase(425, 205, 855, 575, GetColor(45, 40, 35), TRUE);

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 上側のアクセントライン
    DrawBoxBase(425, 205, 855, 215, GetColor(210, 170, 90), TRUE);

    // 下側のアクセントライン
    DrawBoxBase(425, 565, 855, 575, GetColor(210, 170, 90), TRUE);

    // ---------------------------------------------------------------
    // CLEAR!
    // ---------------------------------------------------------------
    DrawStringCenterBase(
        640.0f,
        222.0f,
        "CLEAR!",
        GetColor(255, 255, 255),
        Master::mpFontManager->GetHandle(FontId::SubTitle)
    );

    // ---------------------------------------------------------------
    // ステージ選択ボタンの背景（ボタンの中心と大きさから計算）
    // ---------------------------------------------------------------
    const float buttonLeft = BUTTON_CENTER_X - BUTTON_WIDTH * 0.5f;
    const float buttonTop = BUTTON_CENTER_Y - BUTTON_HEIGHT * 0.5f;
    const float buttonRight = BUTTON_CENTER_X + BUTTON_WIDTH * 0.5f;
    const float buttonBottom = BUTTON_CENTER_Y + BUTTON_HEIGHT * 0.5f;

    DrawBoxBase(buttonLeft, buttonTop, buttonRight, buttonBottom, GetColor(80, 80, 80), TRUE);
    DrawBoxBase(buttonLeft, buttonTop, buttonRight, buttonBottom, GetColor(255, 255, 255), FALSE);

    // ボタン文字はmpStageSelectUIのUITextで描画する

    // ---------------------------------------------------------------
    // アイテム
    // ---------------------------------------------------------------
    const int labelFont = Master::mpFontManager->GetHandle(FontId::Default);

    for (int i = 0; i < 3; i++)
    {
        if (mpCollectibleUI[i] == nullptr)
        {
            continue;
        }

        // アイテムごとに少し時間差をつける
        const float startTime = i * 0.15f;

        if (mfItemTimer < startTime)
        {
            continue;
        }

        // 0.0 〜 1.0
        const float animationDuration = 0.35f;
        float rate = (mfItemTimer - startTime) / animationDuration;

        if (rate > 1.0f)
        {
            rate = 1.0f;
        }

        // 小さい状態から元の大きさへ（基準の拡大率 → 実画面の拡大率）
        const float scale = ToScreenLength(0.2f + 0.3f * rate);

        mpCollectibleUI[i]->GetTransform().SetScale(
            VGet(scale, scale, 1.0f)
        );

        mpCollectibleUI[i]->Draw();

        // 獲得状態（アイテムの真下に中央揃え）
        const float labelCenterX = ITEM_START_X + i * ITEM_SPACING;

        if (mbIsCollected[i])
        {
            DrawStringCenterBase(
                labelCenterX, ITEM_LABEL_Y,
                "獲得",
                GetColor(255, 255, 255),
                labelFont
            );
        }
        else
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);

            DrawStringCenterBase(
                labelCenterX, ITEM_LABEL_Y,
                "未獲得",
                GetColor(160, 160, 160),
                labelFont
            );

            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }
}

ClearUI::~ClearUI()
{
    if (mMessageFontHandle != -1)
    {
        DeleteFontToHandle(mMessageFontHandle);
        mMessageFontHandle = -1;
    }
}