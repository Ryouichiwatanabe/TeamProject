#include "ClearCollectibleUI.h"

void ClearCollectibleUI::Init()
{
    SetEnabled(false);
}

void ClearCollectibleUI::SetCollected(bool _isCollected)
{
    SetEnabled(_isCollected);
}

void ClearCollectibleUI::SetGraph(const std::string& _filename)
{
    LoadUIGraph(_filename);
}