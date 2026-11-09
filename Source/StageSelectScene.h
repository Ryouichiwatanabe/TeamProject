#pragma once
#include "Scene.h"
#include "GameConfig.h"

#include <array>
#include <string>
#include <DxLib.h>

class UI;

class StageSelectScene : public Scene
{
public:
    void Initialize() override;
    void Update(float _deltaTime) override;
    void Draw() override;
    void Finalize() override;

private:
    // 背景画像(notFoundation.png)上のピクセル座標
    struct Point
    {
        float x;
        float y;
    };

    // -- 生成 -- //
    void CreateBackground();
    void CreateStageCard(int _stage);                                   // 料理枠1つ分
    void CreateDishName(int _stage, Point _center, bool _isCompleted);
    void CreateDish(int _stage, Point _center, bool _isCompleted);
    void CreateItems(int _stage, Point _center);

    // -- 配置の補助 -- //
    void PlaceImage(UI* _ui, float _x, float _y, int _layer) const;     // レイヤーと位置
    void FitImage(UI* _ui, float _width, float _height) const;          // 比率を保って枠内に収める
    void StretchImage(UI* _ui, float _width, float _height) const;      // 比率を無視して引き伸ばす
    VECTOR ToUIPosition(float _x, float _y) const;                      // 背景画像の座標 → UIのPosition

private:
    UI* mpBackground = nullptr;

    float mfBgScaleX = 1.0f;    // 背景画像 → 画面 の拡大率
    float mfBgScaleY = 1.0f;

    std::array<UI*, GameConfig::STAGE_COUNT> mpDishPanels{};    // クリック判定も兼ねる [ステージ番号 - 1]
};