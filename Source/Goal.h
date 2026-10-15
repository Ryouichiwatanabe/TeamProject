#pragma once

#include "GameObject.h"

class Player;

class Goal : public GameObject
{
public:
    void Init() override;
    void InitComponent() override;
    void Update(float _deltaTime) override;
    void Draw() override;

    bool IsPlayerTouching();

    bool IsShapeMatched(Player* _player) const;
    bool IsWithinDistance(Player* _player);

private:
    bool mShape[3][3] =
    {
        { false, false, false },
        { false, true,  false },
        { false, false, false }
    };

protected:
    const char* GetModelFilename() const override
    {
        return "Resource/Help.png";
    }
};