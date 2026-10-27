#pragma once

#include "UI.h"
#include "FloatingMotion.h"

#include <string>

class ClearCollectibleUI;

class ClearUI : public UI
{
public:
    ClearUI() = default;

    ClearUI(
        const std::string& _filename0,
        const std::string& _filename1,
        const std::string& _filename2
    );

    ~ClearUI() = default;

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetCollected(
        int _index,
        bool _isCollected
    );

private:
    ClearCollectibleUI* mpCollectibleUI[3] = {};

    std::string mCollectibleFilename[3];

    bool mbIsCollected[3] = {};

    float mfItemTimer = 0.0f;

    FloatingMotion mClearTextMotion;

    UI* mpStageSelectUI = nullptr;
};