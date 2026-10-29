#pragma once
#include "UI.h"
#include <string>

class Stage;

class UIBackground : public UI
{
public:
	UIBackground() = default;
	~UIBackground() override; // ★ = default を外して cpp 側で破棄する

	void Init() override;
	void Update() override;
	void Draw() override;

	// Stage クラスを受け取って背景を設定する
	void SetStage(const Stage* _stage);
	void SetStage(const Stage& _stage);

	// 直接番号を指定して設定する場合
	void SetStageByNumber(int _stageNumber);

	int GetCurrentStageNumber() const { return mnCurrentStageNumber; }

	// 外部からハンドルを参照したい場合用
	int GetGraphHandle() const { return mGraphHandle; }

private:
	std::string BuildBackgroundPath(int _stageNumber) const;
	void LoadBackgroundGraph(const std::string& _path); // ★ 画像読み込み用

private:
	int mnCurrentStageNumber = -1;
	int mGraphHandle = -1; // 背景画像ハンドル
};