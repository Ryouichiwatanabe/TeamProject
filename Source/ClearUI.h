#pragma once

#include "UI.h"
#include "FloatingMotion.h"

class ClearCollectibleUI;

class ClearUI : public UI
{
public:
    ClearUI() = default;
    ~ClearUI() = default;

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetCollectibleUI(
        int _index,
        ClearCollectibleUI* _ui
    );

    void SetCollected(
        int _index,
        bool _isCollected
    );

private:
    ClearCollectibleUI* mpCollectibleUI[3] = {};

    bool mbIsCollected[3] = {};

    float mfItemTimer = 0.0f;

    FloatingMotion mClearTextMotion;
};