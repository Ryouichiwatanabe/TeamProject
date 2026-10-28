#pragma once

/* memo : 
* ゲームの重要な各種値の設定クラス
*/
class GameConfig
{
public:
	static constexpr float CELL_SIZE = 80.0f;	// マスの大きさ(例 1.0fの場合、1.0f × 1.0fのマス目になる)

	static constexpr int PLAYER_PIECE_SIZE = 3;	

	static constexpr float GLAVITY = 9.8f;				// 重力加速度
	static constexpr float MAX_FALLSPEED = 49.0f;		// 落下速度の上限

	// -- ステージ・アイテム -- //
	static constexpr int STAGE_COUNT = 8;                                   // ステージ総数
	static constexpr int MAX_ITEMS_PER_STAGE = 3;                                   // 1ステージのアイテム上限

	// -- 物理・回転・倒れ込み用定数 -- //
	static constexpr float ALLOWED_PENETRATION = CELL_SIZE * 0.1f;	// 回転判定における許容めり込み量
	static constexpr float ROTATION_STEP_DEG = 5.0f;	// 1回の回転角度（度数法）
	static constexpr float ROTATION_SHIFT_X = 8.0f;		// 回転入力時の横移動補正量
	static constexpr float TOPPLE_SPEED_DEG = 3.0f;		// 接地倒れ込み時の角速度（度数法）
	static constexpr float STABLE_GROUND_THRESHOLD_X = 16.0f;	// 安定接地と判定する重心・支点の許容誤差

	// -- 振動抑制（ビクビク軽減）用ダンピング定数 -- //
	static constexpr float COLLISION_DAMPING = 0.75f;	// 衝突押し戻し緩和係数（オーバーシュート抑制）
	static constexpr float PENETRATION_DEADZONE = 0.3f;	// 微小めり込み不感帯（ジッター抑制）
	static constexpr float INPUT_TOPPLE_COOLDOWN = 0.05f;// Key input priority window: suppresses topple rotation (seconds)
};