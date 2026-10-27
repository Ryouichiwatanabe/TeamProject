#pragma once
#include "GameObject.h"
#include <array>

#include "PlayerController.h"

#include "GameConfig.h"

#include <DxLib.h>

class PlayerPiece;
class PlayerShapeUI;	// UI上の入力を取得する用

class Player : public GameObject
{
public:
	void Init() override;
	void InitComponent() override;

	void Update(float _deltaTime) override;	
	void Draw() override;

	void BeginCollisionResolution() override;	// 補正値のリセット
	void EndCollisionResolution() override;		// 補正値の適用

	void Move(VECTOR _direction);			// 移動処理(Controllerで呼ぶ)
	void Rotation(float _rotateDirection);	// 回転処理(Controllerで呼ぶ)

	void ToggleDeformationUI();				// 変形UIの有効状態を切り替える(Controllerで呼ぶ)

	void AddCollisionCorrection(VECTOR _correction);	// 補正値を一旦集める処理(PlayerPieceで呼ぶ)

	void SetShapeUI(PlayerShapeUI* _ui) { mpShapeUI = _ui; }	// PlayerShapeUIのポインタを取得(Player変形情報の取得のため)

	PlayerPiece* GetPlayerPiece(int _column, int _row) { return mPieces[_column][_row]; }

	void ApplyUISelectionToPlayerPiece();	// PlayerPieceUIの選択状態をPlayerPieceに適用させる

private:
	// -- PlayerPieceの生成 -- //
	void SetupPieces();
	void CreatePiece(int _column, int _row);

	// -- リスポーンチェッカー & 処理 -- //
	void CheackRespawn();
	void Respawn();

	// -- 各Pieceの補正値を集約 & 適用 -- //
	void ResetCollisionCorrections();
	VECTOR CalculateCollisionCorrection();
	void ApplyCollisionCorrection();

	// -- Player座標補正後にPieceの座標を更新 -- //
	void UpdatePiecePosition();
	void UpdatePieceRotation();

	// -- Pieceの接地判定をPlayerに適用 -- //
	void OnPieceGrounded();
protected:
	const char* GetModelFilename() const override {
		return "Resource/Player3.png";
	}


private:
	PlayerPiece* mPieces[GameConfig::PLAYER_PIECE_SIZE][GameConfig::PLAYER_PIECE_SIZE]{};

	PlayerController mController;

	PlayerShapeUI* mpShapeUI;	

	std::vector<VECTOR> mCollisionCorrections;	// Pieceの衝突解決の補正値を一時的に集めるコンテナ
	
	float mfSpeed = 10.0f;		// 移動スピード
	float mfAngle = 0.0f;		// 回転の角度(-180 ~ 180)
	float mfRotationPower = 5.0f;	// 回転力(単位は度数)
	VECTOR mvRespawnPos = VGet(200.0f, 200.0f, 0.0f);	// リスポーンの座標

	bool mbIsPinched = false; // 挟まれているかフラグ
};