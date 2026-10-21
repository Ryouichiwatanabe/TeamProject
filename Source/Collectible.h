#pragma once

#include "GameObject.h"
#include <string>

class Collectible : public GameObject
{
public:
    Collectible() = default;
    Collectible(
        const std::string& _filename,
        int _itemIndex
    );

    ~Collectible() = default;

    void Init() override;
    void InitComponent() override;

    void Update(float _deltaTime) override;
    void Draw() override;

    bool IsPickedUp() const
    {
        return mbIsPickedUp;
    }

    int GetItemIndex() const
    {
        return mItemIndex;
    }

private:

    bool mbIsCollected = false;
    bool mbIsPickedUp = false;

    int mItemIndex = -1;

    std::string mFilename;

    float mPickupTimer = 0.0f;
    float mPickupDuration = 0.25f;
};