#pragma once

class StageClearData
{
public:
    static void SetStageCleared(
        int _stageNumber,
        bool _isCleared
    );

    static bool IsStageCleared(
        int _stageNumber
    );

private:
    static bool mbIsStageCleared[8];
};