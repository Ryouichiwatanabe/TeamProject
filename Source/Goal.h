#pragma once

#include "StageBlock.h"

class Player;

class Goal : public StageBlock
{
public:
    void Init() override;

    bool IsPlayerTouching();

    bool IsShapeMatched(Player* _player) const;
    bool IsWithinDistance(Player* _player);

protected:
    const char* GetModelFilename() const override
    {
        return "Resource/Stage.png";
    }

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