#pragma once
#include "GameObject.h"
#include "PlayerController.h"
#include "Box2D.h"
#include "GameConfig.h"
#include "CollisionManager.h"	// CollisionInfo

#include <array>
#include <vector>

class PlayerPiece;
class Transform;

// -- 形状マスク : [column][row] の3×3。true = ブロックあり -- //
using PlayerShapeMask = std::array<
	std::array<bool, GameConfig::PLAYER_PIECE_SIZE>,
	GameConfig::PLAYER_PIECE_SIZE>;

// -- 形状変更の結果 -- //
enum class ShapeChangeResult
{
	Success,		// 変形成功
	BaseRemoved,	// 基盤ブロックが外されている
	Disconnected,	// ブロックがひとつの塊になっていない
	Overlapping,	// 追加したブロックが地形にめり込む
};

// -- 各Pieceの接触情報(Pieceが報告し、Playerが集める) -- //
struct PieceContact {
	PlayerPiece* mpPiece = nullptr;
	GameObject* mpOtherBlock = nullptr;
	CollisionInfo mCollisionInfo;
	VECTOR mvPieceWorldPos = VGet(0.0f, 0.0f, 0.0f);
};

/* memo :
* プレイヤー本体。3×3の PlayerPiece(ブロック)をまとめて動かす「ハブ」。
* Player の座標 = 回転軸Piece の座標。各Pieceは「回転軸Pieceからの相対座標」を持つ。
*
* ■ 処理の置き場所
*   Player.cpp         : 生成 / 毎フレームの流れ / Pieceの座標同期 / リスポーン
*   PlayerRotation.cpp : 回転(めり込みチェック・横ずらし・床キック) / 回転軸 / 姿勢の当たり判定
*   PlayerGround.cpp   : 衝突の押し戻し / 接地・倒れ込み
*   PlayerShape.cpp    : 形状の設定 / 変形UIからの形状変更
*
* ■ 1フレームの流れ
*   1. Update()                   : 入力(回転) → 重力
*   2. CollisionManager::Update() : 当たり判定
*   3. BeginCollisionResolution() : 前フレームの報告をクリア
*   4. PlayerPiece::ResolveCollision() : 各Pieceが押し戻し量・接触情報を Player に報告
*   5. EndCollisionResolution()   : 押し戻し → 接地 / 倒れ込み
*/
class Player : public GameObject
{
public:
	// ================= 基本 (Player.cpp) ================= //
	void Init() override;
	void InitComponent() override;

	void Update(float _deltaTime) override;
	void Draw() override;

	void UpdatePieceTransforms();			// 全Pieceのワールド座標を Player の姿勢に合わせる
	VECTOR CalculateCenterOfMass() const;	// 有効Pieceの重心(ワールド座標)

	// ================= 回転 (PlayerRotation.cpp) ================= //
	// めり込まない姿勢が見つかれば回転して true。見つからなければ何もせず false
	bool TryRotateWithShift(float _rotateDirection);

	// ================= 衝突・接地 (PlayerGround.cpp) ================= //
	void BeginCollisionResolution() override;	// 前フレームの報告をクリア
	void EndCollisionResolution() override;		// 押し戻し → 接地/倒れ込み

	// PlayerPiece から報告を受け取る
	void AddCollisionCorrection(const VECTOR& _correction);
	void AddPieceContact(const PieceContact& _contact);

	bool IsToppling() const { return mbIsToppling; }
	bool IsGrounded() const { return mbIsGrounded; }

	// ================= 形状 (PlayerShape.cpp) ================= //
	void SetShape(int _shapeType);	// プリセット形状(0:T字 1:十字 2:3×3 3:2×2)

	// 変形UIから呼ぶ
	void BeginShapeEdit();		// 基盤ブロックを決定し、Playerを停止する
	void EndShapeEdit();		// 停止を解除する
	bool IsShapeEditing() const { return mbIsShapeEditing; }

	// 画面の見た目の向き(回転を打ち消した向き)での形状・基盤マス
	PlayerShapeMask GetScreenShape() const;
	void GetScreenBaseCell(int& _column, int& _row) const;
	ShapeChangeResult TryChangeShapeFromScreen(const PlayerShapeMask& _screenShape);

protected:
	const char* GetModelFilename() const override {
		return "Resource/GameObject/Player.png";
	}

private:
	// ----------------- 基本 (Player.cpp) ----------------- //
	void SetupPieces();
	void CreatePiece(int _column, int _row);
	void CheckRespawn();
	void Respawn();
	void PrintDebugInfo();
	Transform* GetTransform() const;	// const関数からも使えるTransform取得

	// ----------------- 回転 (PlayerRotation.cpp) ----------------- //
	// 指定した姿勢で置いたとき、ステージブロックとどう当たるか
	struct PoseCollisionResult {
		bool isColliding = false;
		float maxPenetration = 0.0f;					// 一番深いめり込み量
		VECTOR correction = VGet(0.0f, 0.0f, 0.0f);		// 押し出しに必要な移動量
		bool hasFloorCollision = false;
		bool hasWallCollision = false;
		bool hasCeilingCollision = false;
	};
	PoseCollisionResult TestCollisionAtPose(const VECTOR& _pos, float _rotZ) const;
	PoseCollisionResult TestCollisionAtPose(const VECTOR& _pos, float _rotZ, const PlayerShapeMask& _shape) const;

	bool IsPoseAcceptable(const PoseCollisionResult& _result) const;	// 許容範囲のめり込みか
	bool CanFloorKick(const PoseCollisionResult& _result) const;		// 床だけに当たっているか
	void ApplyRotatedPose(const VECTOR& _pos, float _rotZ);				// 回転後の姿勢を確定

	void RecalculateRotationPivot();	// 有効Pieceのいずれか一つを回転軸(=Playerの原点)にする

	// ----------------- 衝突・接地 (PlayerGround.cpp) ----------------- //
	VECTOR CalculateCollisionCorrection() const;
	void ApplyCollisionCorrection();

	void ProcessGroundAndTopple();
	std::vector<PieceContact> CollectGroundContacts() const;		// 下から支えている接触だけ集める
	bool IsStableOnGround(const std::vector<PieceContact>& _groundContacts) const;
	void SnapRotationIfNearlyFlat();								// 90度単位からの微小なズレを消す
	void Topple(const std::vector<PieceContact>& _groundContacts);	// 支点まわりに少し倒す
	void SetGroundedState(bool _grounded);							// 接地フラグ + 重力の接地状態

	// ----------------- 形状 (PlayerShape.cpp) ----------------- //
	PlayerShapeMask GetLocalShape() const;	// 現在の形状(Player基準の向き)
	void ApplyLocalShape(const PlayerShapeMask& _shape);
	int GetQuarterTurns() const;			// 現在の回転が90度の何回分か(0〜3)
	void SelectBaseCell();					// 基盤ブロックの決定
	static bool IsConnected(const PlayerShapeMask& _shape, int _startColumn, int _startRow);

private:
	PlayerController mController;

	PlayerPiece* mPieces[GameConfig::PLAYER_PIECE_SIZE][GameConfig::PLAYER_PIECE_SIZE]{};	// [column][row]

	// -- PlayerPieceからの報告(毎フレームクリア) -- //
	std::vector<VECTOR> mCollisionCorrections;	// 押し戻し量
	std::vector<PieceContact> mPieceContacts;	// 接触情報

	VECTOR mvRespawnPos = VGet(200.0f, 200.0f, 0.0f);	// リスポーン座標

	// -- 接地・倒れ込み -- //
	bool mbIsGrounded = false;		// 安定して接地しているか
	bool mbIsToppling = false;		// 倒れ込み中か
	float mfInputCooldown = 0.0f;	// 回転入力の直後、倒れ込みを止めておく残り時間

	// -- 形状変更 -- //
	bool mbIsShapeEditing = false;	// 変形UIを開いているか(開いている間は停止)
	int mnBaseColumn = 1;			// 基盤ブロックのマス(Player基準)
	int mnBaseRow = 1;

	// -- 回転軸Piece(Player基準のマス) -- //
	int mnPivotColumn = 1;
	int mnPivotRow = 1;
};
