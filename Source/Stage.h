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

	int GetStageNumber() const { return mStageNumber; }
private:
	void CreateObjects();

	int mStageNumber = 1;

private:
	std::vector<std::vector<int>> mStageData;

	
};