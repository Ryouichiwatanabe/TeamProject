#pragma once
#include "UI.h"
#include <string>
#include <vector>

struct BgDishDef;

/* memo :
* ステージ番号に対応した背景を描画するクラス。
* 料理データがあるステージは「土台 + 取得アイテムに応じた重ね画像」で描く。
* 料理データがないステージ（チュートリアル等）は従来の1枚絵。
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
	void SetPicked(int _slot);		// アイテム取得時に呼ぶ

private:
	std::string BuildBackgroundPath(int _stageNumber) const;

	void LoadSingleGraph(int _stageNumber);	// 従来の1枚絵
	void LoadDishGraphs();					// 料理レイヤー一式
	void DrawDish() const;
	void DrawImage(int _handle, float _x, float _y, float _scale, float _angle) const;

private:
	const BgDishDef* mpDish = nullptr;		// 料理データ（無ければ nullptr）
	int mnFoundationHandle = -1;
	float mfBaseWidth = 1.0f;				// Foundation画像のサイズ（座標の基準）
	float mfBaseHeight = 1.0f;
	std::vector<int> mLayerHandles;			// mpDish->layers と同じ順番
	int mnPickedBits = 0;					// 取得済みスロットのビット
};