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
	void InitPiece();	// Todo : これする場所確定させる

	void Update(float _deltaTime) override;	
	void Draw() override;

	void BeginCollisionResolution() override;	// 補正値のリセット
	void EndCollisionResolution() override;		// 補正値の適用

	void Respawn();	// リスポーン処理

	void Move(VECTOR _direction);			// 移動処理(Controllerで呼ぶ)
	void Rotation(float _rotateDirection);	// 回転処理(Controllerで呼ぶ)

	void AddCollisionCorrection(VECTOR _correction);	// 補正値を一旦集める処理(PlayerPieceで呼ぶ)

	void SetShapeUI(PlayerShapeUI* _ui) { mpShapeUI = _ui; }	// PlayerShapeUIのポインタを取得(Player変形情報の取得のため)
	void ApplyShapeFromUI();									// PlayerShapeUIの情報からPieceの有効状態を変更する処理

	bool IsPieceEnabled(int _column, int _row);
private:

	VECTOR mvRespawnPos = VGet(0.0f, 0.0f, 0.0f);	// リスポーンの座標

	void CreatePiece(int _column, int _row);

	// -- 各Pieceの補正値を集約 & 適用 -- //
	void ResetCollisionCorrections();
	VECTOR CalculateCollisionCorrection();
	void ApplyCollisionCorrection();

	// -- Player座標補正後にPieceの座標を更新 -- //
	void UpdatePiecePosition();
	void UpdatePieceRotation();
protected:
	const char* GetModelFilename() const override {
		return "Resource/Player.png";
	}

private:
	PlayerPiece* mPieces[GameConfig::PLAYER_PIECE_SIZE][GameConfig::PLAYER_PIECE_SIZE]{};

	PlayerController mController;

	PlayerShapeUI* mpShapeUI;	

	std::vector<VECTOR> mCollisionCorrections;	// Pieceの衝突解決の補正値を一時的に集めるコンテナ
	
	float mfSpeed = 10.0f;		// 移動スピード
	float mfAngle = 0.0f;		// 回転の角度(-180 ~ 180)
	float mfRotationPower = 5.0f;	// 回転力(単位は度数)
};