#pragma once

#include "GameObject.h"

class Collectible : public GameObject
{
public:
    Collectible() = default;
    ~Collectible() = default;

    void Init() override;
    void InitComponent() override;

    void Update(float _deltaTime) override;
    void Draw() override;

private:
    bool mbIsCollected = false;
};