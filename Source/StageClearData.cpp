#include "StageClearData.h"

bool StageClearData::mbIsStageCleared[8] = {};
bool StageClearData::mbIsItemCollected[8][3] = {};

bool StageClearData::mbIsTutorialCleared = false;

void StageClearData::SetStageCleared(
    int _stageNumber,
    bool _isCleared
)
{
    if (_stageNumber < 1 || _stageNumber > 8)
        return;

    mbIsStageCleared[_stageNumber - 1] = _isCleared;
}

bool StageClearData::IsStageCleared(
    int _stageNumber
)
{
    if (_stageNumber < 1 || _stageNumber > 8)
        return false;

    return mbIsStageCleared[_stageNumber - 1];
}

void StageClearData::SetItemCollected(
    int _stageNumber,
    int _itemIndex,
    bool _isCollected
)
{
    if (_stageNumber < 1 || _stageNumber > 8)
        return;

    if (_itemIndex < 0 || _itemIndex >= 3)
        return;

    mbIsItemCollected[_stageNumber - 1][_itemIndex] =
        _isCollected;
}

bool StageClearData::IsItemCollected(
    int _stageNumber,
    int _itemIndex
)
{
    if (_stageNumber < 1 || _stageNumber > 8)
        return false;

    if (_itemIndex < 0 || _itemIndex >= 3)
        return false;

    return mbIsItemCollected[_stageNumber - 1][_itemIndex];
}