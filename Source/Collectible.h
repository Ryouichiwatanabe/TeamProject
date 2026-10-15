#pragma once

#include "GameObject.h"

#include <string>

class Collectible : public GameObject
{
public:
    Collectible(const std::string& _filename,int _itemIndex);
    ~Collectible() = default;

    void Init() override;
    void InitComponent() override;

    void Update(float _deltaTime) override;
    void Draw() override;

    int GetItemIndex() const
    {
        return mItemIndex;
    }

    bool IsCollected() const
    {
        return mbIsCollected;
    }

    bool IsPickedUp() const
    {
        return mbIsPickedUp;
    }

private:
    bool mbIsCollected = false;

    float mBaseY = 0.0f;
    float mMoveTime = 0.0f;
    bool mbIsBasePositionSet = false;

    // 取得演出
    float mCollectTime = 0.0f;
    float mCollectDuration = 0.3f;
    bool mbIsCollectEffect = false;

    // アイテム画像
    std::string mFilename;

    int mItemIndex = 0;

    bool mbIsPickedUp = false;
};