#pragma once
#include <string>
#include <vector>
#include <DxLib.h>

class Player;

class Stage
{
public:
    void Load(const std::string& _filename);

    std::string GetCollectibleFilename(int _itemIndex) const;

private:
    void CreateObjects();

private:
    std::vector<std::vector<int>> mStageData;

    // 現在のステージ番号
    int mStageNumber = 0;
};