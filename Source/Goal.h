#pragma once

#include "StageBlock.h"

class Player;

class Goal : public StageBlock
{
public:
    void Init() override;

    bool IsPlayerTouching();

    //bool IsShapeMatched(Player* _player) const;
    bool IsWithinDistance(Player* _player);

private:
    /*bool mShape[3][3] =
    {
        { false, false, false },
        { false, true,  false },
        { false, false, false }
    };*/


};