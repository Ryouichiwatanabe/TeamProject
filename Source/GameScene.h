#pragma once

#include "Scene.h"
#include <cstddef>
#include <memory>
#include "EffectClear.h"

#include "UIBackground.h"
#include "ItemRecord.h"		// StageFlags を使うため

class Player;
class Goal;
class PauseUI;
class ClearUI;
class ItemHUD;

class PlayerShapeUI;

class GameScene : public Scene
{
public:
    void Initialize();
    void Update(float deltaTime);
    void Draw();
    void Finalize();
    void SetStageNumber(int _stageNumber);
    std::unique_ptr<EffectManager> mpEffectManager;

private:
    void UpdatePauseInput();    // 入力に伴い、PauseUIを有効にする
    void UpdateClear();
    void UpdateItems();				// Item の取得状態を確認する
    void UpdateShapeUIToggle();     // 変形キーで変形UIを開閉する

private:
    Goal* mpGoal = nullptr;

    PauseUI* mpPauseUI = nullptr;
    ClearUI* mpClearUI = nullptr;
    ItemHUD* mpItemHUD = nullptr;
    ItemRecord::StageFlags mPickedItems{};		// 今回のプレイの仮取得

    PlayerShapeUI* mpShapeUI = nullptr;
    UIBackground* mpBackgroundUI = nullptr;

    int mnStageNumber = 1;

    bool mbIsClear = false;
    float mClearTimer = 0.0f;

    bool IsPlayingSE = false;
};
