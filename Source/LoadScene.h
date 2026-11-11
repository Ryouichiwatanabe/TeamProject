#pragma once
#include "Scene.h"

class LoadUI;

// 料理と豆腐の世界観に合わせたロード画面
// 見た目は LoadUI にまかせ、ここでは「時間・入力・シーン遷移」だけを扱う
class LoadScene : public Scene
{
public:
	LoadScene() = default;
	~LoadScene() override = default;

	void Initialize() override;				// LoadUI の生成
	void Update(float _deltaTime) override;	// したごしらえ進行とクリック検知
	void Finalize() override;				// 自前で作った画像の解放

private:
	static constexpr float LOAD_DURATION = 1.5f;	// したごしらえ完了までの時間（秒）

	LoadUI* mpLoadUI = nullptr;	// 所有権は UIManager
	float mfTimer = 0.0f;		// 経過タイマー
	bool mbReady = false;		// 準備完了フラグ
};
