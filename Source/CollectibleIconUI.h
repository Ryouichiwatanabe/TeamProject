#pragma once
#include "UI.h"

class CollectibleIconUI : public UI
{
public:
    CollectibleIconUI() = default;
    ~CollectibleIconUI() = default;

    void Init() override;

    void SetCollected(bool _isCollected);
    void SetGraph(const std::string& _filename);
};