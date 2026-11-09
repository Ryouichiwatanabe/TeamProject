#pragma once

#include "StageBlock.h"

class Player;

class Goal : public StageBlock
{
public:
    void Init() override;

    bool IsPlayerTouching();    // Playerと触れたかどうかを示す

protected:
	const char* GetModelFilename() const override
	{
		return "Resource/GameObject/StageBlock/Goal.png";
	}
};