#pragma once

#include "UI.h"
#include <string>

class ClearCollectibleUI : public UI
{
public:
    ClearCollectibleUI() = default;
    ~ClearCollectibleUI() = default;

    void Init() override;

    void SetCollected(bool _isCollected);
    void SetGraph(const std::string& _filename);
};