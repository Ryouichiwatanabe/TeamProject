#pragma once

class Player;

class PlayerController
{
public:
	PlayerController() = default;
	~PlayerController() = default;

	void Initialize(Player* _player);	// 初期処理(主にPlayer設定)
	void Update();

private:
	void UpdateMove();					// 移動キーの入力をPlayerに渡す
	void UpdateOperationChange();		// (Debag用)FakePlayerに操作を切り替える
	void UpdateRotation();				// 回転キーの入力をPlayerに渡す
	void UpdatePlayerDeformationUI();	// 変形キーの入力をPlayerに渡す

private:
	Player* mpPlayer;
};