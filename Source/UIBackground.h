#pragma once
#include "UI.h"
#include <string>

/* memo :
* ステージ番号に対応した背景画像を描画するクラス。
* UIManagerの描画順だとオブジェクトより手前に描かれてしまうため、
* Draw()では何もせず、GameScene::Draw()の最初で DrawBackground() を呼ぶ。
*/
class UIBackground : public UI
{
public:
	UIBackground() = default;
	~UIBackground() override;

	void Init() override;
	void Update() override;
	void Draw() override;			// UIManager経由では描画しない

	void DrawBackground();			// GameSceneから最背面として呼ぶ

	// ステージ番号を指定して背景を設定
	void SetStageByNumber(int _stageNumber);

	int GetCurrentStageNumber() const { return mnCurrentStageNumber; }
	int GetGraphHandle() const { return mGraphHandle; }

private:
	std::string BuildBackgroundPath(int _stageNumber) const;
	void LoadBackgroundGraph(const std::string& _path);

private:
	int mnCurrentStageNumber = -1;
	int mGraphHandle = -1;
};