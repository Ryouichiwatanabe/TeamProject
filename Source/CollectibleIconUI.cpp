#include "CollectibleIconUI.h"

void CollectibleIconUI::Init()
{
    SetEnabled(false);
}

void CollectibleIconUI::SetCollected(bool _isCollected)
{
    SetEnabled(_isCollected);
}

void CollectibleIconUI::SetGraph(const std::string& _filename)
{
    LoadUIGraph(_filename);
}