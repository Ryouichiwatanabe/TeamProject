#pragma once
#include "UI.h"
#include <string>

/* memo :
* ステージ番号に対応した背景画像を描画するクラス。
* UIManagerの描画順だとオブジェクトより手前に描かれてしまうため、
* Draw()では何もせず、GameScene::Draw()の最初で DrawBackground() を呼ぶ。
*/
class StageBackGround : public UI
{
public:
	StageBackGround() = default;
	~StageBackGround() = default;

	void Init() override;
	void Update() override;
	void Draw() override;			// UIManager経由では描画しない

	void DrawBackground();			// GameSceneから最背面として呼ぶ
private:
	std::string BuildBackgroundPath(int _stageNumber) const;
};