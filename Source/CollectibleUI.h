#pragma once

#include "UI.h"

class CollectibleUI : public UI
{
public:
    CollectibleUI() = default;
    ~CollectibleUI() = default;

    void Init() override;

    void SetCollected(bool _isCollected);
};