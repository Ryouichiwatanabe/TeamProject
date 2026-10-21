#include "CollectibleUI.h"

void CollectibleUI::Init()
{
    SetEnabled(true);
}

void CollectibleUI::SetCollected(bool _isCollected)
{
    SetEnabled(_isCollected);
}