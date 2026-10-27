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
	void UpdateRotation();				// 回転キーの入力をPlayerに渡す

private:
	Player* mpPlayer = nullptr;
	float mfPrevRotation = 0.0f;
	float mfRepeatTimer = 0.0f;
};