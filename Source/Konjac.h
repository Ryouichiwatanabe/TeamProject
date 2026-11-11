#pragma once
#include "StageBlock.h"

#include <DxLib.h>	// VECTOR用

class Player;
class PlayerPiece;

// 上に乗るとプルンと揺れてプレイヤーを跳ね上げる「こんにゃく」ブロック
// memo : 旧キッチンスポンジ。Stage.cpp から参照されているためクラス名は据え置き
class Konjac : public StageBlock
{
public:
	~Konjac() override = default;

	void Init() override;
	void Update(float _deltaTime) override;
	void Draw() override;

protected:
	const char* GetModelFilename() const override
	{
		return "Resource/GameObject/StageBlock/Konjac.png";
	}

private:
	// -- 跳ね上げ -- //
	void    TryStartJump();								// 上に乗ったプレイヤーがいれば跳ね上げ開始
	Player* FindPlayerOnTop();							// 真上に乗っているプレイヤーを探す
	void    UpdateJumpCarry(float _deltaTime);			// 跳ね上げ中のプレイヤーを上へ運ぶ
	bool    HasCeilingAbovePlayer(Player* _player, float _stepY);	// 移動先に天井があるか

	// -- 描画 -- //
	void DrawBounce();									// 揺れ中の描画(底辺固定で伸縮)

private:
	// -- 跳ね上げ -- //
	static constexpr float INITIAL_JUMP_SPEED = 1500.0f;	// 跳ね上げる初速(px/秒)
	static constexpr float JUMP_DECEL = 2100.0f;			// 上昇中の減速度(px/秒^2)
	static constexpr float COOLDOWN_TIME = 0.35f;			// 連続発動を防ぐクールタイム(秒)

	// -- 乗っている判定の許容幅 -- //
	static constexpr float ON_TOP_RANGE_X_RATE = 0.85f;	// 横方向の許容幅(CELL_SIZE比)
	static constexpr float ON_TOP_ABOVE = 12.0f;			// 上面より上の許容(px)
	static constexpr float ON_TOP_BELOW = 28.0f;			// 上面より下の許容(px)

	// -- プルンプルン演出 -- //
	static constexpr float BOUNCE_ANIM_TIME = 0.45f;		// 揺れる秒数
	static constexpr float BOUNCE_FREQUENCY = 28.0f;		// 揺れの速さ
	static constexpr float BOUNCE_DAMPING = 6.0f;			// 揺れの減衰の強さ
	static constexpr float BOUNCE_STRETCH_Y = 0.38f;		// 縦の伸縮量
	static constexpr float BOUNCE_STRETCH_X = 0.22f;		// 横の伸縮量(縦と逆向き)

	Player* mpJumpingPlayer = nullptr;	// 跳ね上げ中のプレイヤー
	float   mfJumpSpeedY = 0.0f;		// 現在の上昇速度
	float   mfBounceTimer = 0.0f;		// 揺れ演出の残り時間
	float   mfCooldown = 0.0f;			// 再発動までのクールタイム

	int     mnGraphHandle = -1;							// 揺れ描画用の画像ハンドル
	VECTOR  mvGraphSize = VGet(0.0f, 0.0f, 0.0f);		// 画像サイズ
};
