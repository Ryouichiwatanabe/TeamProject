#pragma once

class GameProgress
{
public:
    // -- チュートリアルの終了状態のアクセサ -- //
    static void SetTutorialCleared(bool _isCleared) { mbIsTutorialCleared = _isCleared; }
    static bool IsTutorialCleared() { return mbIsTutorialCleared; }

private:
    static bool mbIsTutorialCleared;    // チュートリアルの終了を示す
};