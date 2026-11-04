#include "StageClearData.h"

bool StageClearData::mbIsStageCleared[8] = {};

void StageClearData::SetStageCleared(
    int _stageNumber,
    bool _isCleared
)
{
    if (_stageNumber < 1 || _stageNumber > 8)
    {
        return;
    }

    mbIsStageCleared[_stageNumber - 1] = _isCleared;
}

bool StageClearData::IsStageCleared(
    int _stageNumber
)
{
    if (_stageNumber < 1 || _stageNumber > 8)
    {
        return false;
    }

    return mbIsStageCleared[_stageNumber - 1];
}