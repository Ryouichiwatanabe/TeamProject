#include "Player.h"

/* memo :
* Player の「回転」部分。
*
* ■ TryRotateWithShift の考え方
*   回す前に「回した後の姿勢」で当たり判定をして、めり込むなら回さない。
*   試す姿勢は次の順番:
*     候補1 : 回転方向へ少し横にずらして回す(転がるように進むため)
*     候補2 : その場で回す(壁際でずらせないとき用)
*   それぞれ
*     (a) そのまま置けるならOK
*     (b) 床だけに当たっているなら、押し上げて置けるか試す(床キック)
*   全部ダメなら回転キャンセル。
*/

// -- Module -- //
#include "Transform.h"
#include "Collider.h"

// -- ステージブロックの取得用 -- //
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "PlayerPiece.h"

#include "CollisionManager.h"
#include "GameConfig.h"
#include "PlayerMath.h"

#include <DxLib.h>
#include <cmath>
#include <cfloat>
#include <vector>

namespace
{
	// 1つの接触を「床 / 天井 / 壁」に分類した結果
	struct ContactClassification
	{
		float penetration = 0.0f;					// めり込み量
		VECTOR correction = VGet(0.0f, 0.0f, 0.0f);	// 押し出し量
		bool isFloor = false;
		bool isCeiling = false;
		bool isWall = false;
	};

	// ブロックの左右どちらかの面に、隣のブロックがぴったりくっついているか
	// (くっついている面 = 床や壁の「つなぎ目」で、本来は外から触れない面)
	bool HasNeighborBlock(const Box2D& _blockBox, float _side, const std::vector<Box2D>& _blocks)
	{
		constexpr float SAME_POSITION_EPS = 1.0f;
		const float expectedX = _blockBox.position.x + _side * (_blockBox.half.x * 2.0f);

		for (const Box2D& other : _blocks)
		{
			if (std::fabsf(other.position.x - expectedX) <= SAME_POSITION_EPS &&
				std::fabsf(other.position.y - _blockBox.position.y) <= SAME_POSITION_EPS)
				return true;
		}
		return false;
	}

	// 接触を「床 / 天井 / 壁」に分類する。基本は SAT の法線で判断する。
	//
	// 例外 : 床ブロック同士のつなぎ目
	//   傾いたPieceの角がつなぎ目に少し入ると、SATは「横向きの法線 = 壁」を返し、
	//   回転がキャンセルされてしまう(ゲーム開始直後に右回転できなかった原因)。
	//   横向きの法線でも、その面に隣のブロックがくっついている(=つなぎ目)なら、
	//   Pieceの位置に応じて床 / 天井として扱い、押し出しも真上 / 真下で計算する。
	//   ※本物の段差の側面には隣のブロックが無いので、従来どおり「壁」になる
	//     (ここまで床扱いすると、段差で一気に押し上げられてビクビクする)
	ContactClassification ClassifyContact(
		const VECTOR& _pieceCenter,
		const Box2D& _pieceBox,
		const Box2D& _blockBox,
		const CollisionInfo& _info,
		const std::vector<Box2D>& _blocks)
	{
		ContactClassification result;
		result.penetration = _info.penetration;
		result.correction = VScale(_info.normal, _info.penetration);

		const bool isSideNormal = std::fabsf(_info.normal.x) > 0.5f;
		if (isSideNormal)
		{
			// 法線はブロック→Piece の向き。normal.x < 0 なら Piece はブロックの左側
			const float side = (_info.normal.x < 0.0f) ? -1.0f : 1.0f;

			if (HasNeighborBlock(_blockBox, side, _blocks))
			{
				const float blockTop = _blockBox.position.y - _blockBox.half.y;
				const float blockBottom = _blockBox.position.y + _blockBox.half.y;
				const float pieceExtentY = _pieceBox.ProjectToAxis(VGet(0.0f, 1.0f, 0.0f));	// 中心から上下端までの高さ

				if (_pieceCenter.y <= blockTop)
				{
					// 床 : Pieceの下端がブロック上面にめり込んだ量
					result.penetration = (_pieceCenter.y + pieceExtentY) - blockTop;
					result.correction = VGet(0.0f, -result.penetration, 0.0f);
					result.isFloor = true;
					return result;
				}
				if (_pieceCenter.y >= blockBottom)
				{
					// 天井 : Pieceの上端がブロック下面にめり込んだ量
					result.penetration = blockBottom - (_pieceCenter.y - pieceExtentY);
					result.correction = VGet(0.0f, result.penetration, 0.0f);
					result.isCeiling = true;
					return result;
				}
			}
		}

		// 通常 : SATの法線で分類
		if (_info.normal.y < -0.5f)
			result.isFloor = true;
		else if (_info.normal.y > 0.5f)
			result.isCeiling = true;

		if (isSideNormal)
			result.isWall = true;

		return result;
	}
}

// ============================================================ //
//  回転
// ============================================================ //
bool Player::TryRotateWithShift(float _rotateDirection)
{
	if (_rotateDirection == 0.0f)
		return false;

	auto transform = GetTransform();
	if (!transform)
		return false;

	const VECTOR currentPos = transform->GetPosition();
	const float candidateAngle =
		transform->GetRotation().z + _rotateDirection * PlayerMath::DegToRad(GameConfig::ROTATION_STEP_DEG);

	const VECTOR candidates[2] = {
		VAdd(currentPos, VGet(_rotateDirection * GameConfig::ROTATION_SHIFT_X, 0.0f, 0.0f)),	// 候補1 : 横にずらす
		currentPos																				// 候補2 : その場
	};

	for (const VECTOR& testPos : candidates)
	{
		const PoseCollisionResult result = TestCollisionAtPose(testPos, candidateAngle);

		// (a) そのまま置ける
		if (IsPoseAcceptable(result))
		{
			ApplyRotatedPose(testPos, candidateAngle);
			return true;
		}

		// (b) 床キック : 床だけに当たっているなら押し上げて置けるか試す
		if (CanFloorKick(result))
		{
			const VECTOR liftedPos = VAdd(testPos, result.correction);
			if (IsPoseAcceptable(TestCollisionAtPose(liftedPos, candidateAngle)))
			{
				ApplyRotatedPose(liftedPos, candidateAngle);
				return true;
			}
		}
	}

	// どの候補もめり込む → 回転キャンセル
	return false;
}

bool Player::IsPoseAcceptable(const PoseCollisionResult& _result) const
{
	return !_result.isColliding || _result.maxPenetration <= GameConfig::ALLOWED_PENETRATION;
}

bool Player::CanFloorKick(const PoseCollisionResult& _result) const
{
	return _result.hasFloorCollision && !_result.hasCeilingCollision && !_result.hasWallCollision;
}

void Player::ApplyRotatedPose(const VECTOR& _pos, float _rotZ)
{
	auto transform = GetTransform();
	transform->SetPosition(_pos);
	transform->SetRotation(VGet(0.0f, 0.0f, _rotZ));

	mbIsToppling = false;
	mfInputCooldown = GameConfig::INPUT_TOPPLE_COOLDOWN;	// 直後の倒れ込みで入力が打ち消されないように
	UpdatePieceTransforms();
}

// ============================================================ //
//  姿勢の当たり判定
// ============================================================ //
Player::PoseCollisionResult Player::TestCollisionAtPose(const VECTOR& _pos, float _rotZ) const
{
	return TestCollisionAtPose(_pos, _rotZ, GetLocalShape());
}

Player::PoseCollisionResult Player::TestCollisionAtPose(const VECTOR& _pos, float _rotZ, const PlayerShapeMask& _shape) const
{
	PoseCollisionResult result;

	auto scene = Master::mpSceneManager ? Master::mpSceneManager->GetCurrentScene() : nullptr;
	auto objectManager = scene ? scene->GetObjectManager() : nullptr;
	if (!objectManager)
		return result;

	// -- ステージブロックの箱を先に集めておく -- //
	std::vector<Box2D> blockBoxes;
	for (auto obj : objectManager->GetObjects())
	{
		if (!obj || obj->GetTag() != Tag::BLOCK)
			continue;

		auto blockCollider = obj->GetModule<Collider>();
		if (!blockCollider || !blockCollider->IsEnabled())
			continue;

		blockBoxes.push_back(blockCollider->GetBox2D());
	}

	const VECTOR halfPieceSize = VGet(GameConfig::CELL_SIZE / 2.0f, GameConfig::CELL_SIZE / 2.0f, 0.0f);

	for (int r = 0; r < GameConfig::PLAYER_PIECE_SIZE; ++r)
	{
		for (int c = 0; c < GameConfig::PLAYER_PIECE_SIZE; ++c)
		{
			auto piece = mPieces[c][r];
			if (!piece || !_shape[c][r])
				continue;

			// その姿勢のときのPieceの箱
			const VECTOR pieceWorldPos = PlayerMath::LocalToWorld(_pos, _rotZ, piece->GetLocalPosition());
			const Box2D pieceBox = Box2D::Make(pieceWorldPos, halfPieceSize, _rotZ);

			for (const Box2D& blockBox : blockBoxes)
			{
				CollisionInfo info;
				if (!CollisionManager::TestBoxBox(pieceBox, blockBox, info))
					continue;

				const ContactClassification contact = ClassifyContact(pieceWorldPos, pieceBox, blockBox, info, blockBoxes);

				result.isColliding = true;
				result.hasFloorCollision |= contact.isFloor;
				result.hasCeilingCollision |= contact.isCeiling;
				result.hasWallCollision |= contact.isWall;

				if (contact.penetration > result.maxPenetration)
					result.maxPenetration = contact.penetration;

				// 押し出し量 : 横は一番大きいもの、縦は一番上向き(-Y)のもの
				if (std::fabsf(contact.correction.x) > std::fabsf(result.correction.x))
					result.correction.x = contact.correction.x;
				if (contact.correction.y < result.correction.y)
					result.correction.y = contact.correction.y;
			}
		}
	}

	return result;
}

// ============================================================ //
//  回転軸
// ============================================================ //
void Player::RecalculateRotationPivot()
{
	constexpr int N = GameConfig::PLAYER_PIECE_SIZE;

	// -- 1. 有効Pieceの重心(マス単位)を求める -- //
	float sumColumn = 0.0f;
	float sumRow = 0.0f;
	int activeCount = 0;

	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			if (mPieces[c][r] == nullptr || !mPieces[c][r]->IsActive())
				continue;

			sumColumn += static_cast<float>(c);
			sumRow += static_cast<float>(r);
			++activeCount;
		}
	}

	if (activeCount == 0)
		return;

	const float centroidColumn = sumColumn / activeCount;
	const float centroidRow = sumRow / activeCount;

	// -- 2. 重心に一番近い有効Pieceを回転軸に選ぶ -- //
	//    (同じ距離なら3×3の中央に近いほう → 走査順で先のほう)
	int bestColumn = -1;
	int bestRow = -1;
	float bestDistSq = FLT_MAX;
	float bestCenterDistSq = FLT_MAX;

	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			if (mPieces[c][r] == nullptr || !mPieces[c][r]->IsActive())
				continue;

			const float dc = c - centroidColumn;
			const float dr = r - centroidRow;
			const float distSq = dc * dc + dr * dr;

			const float cc = static_cast<float>(c - N / 2);
			const float cr = static_cast<float>(r - N / 2);
			const float centerDistSq = cc * cc + cr * cr;

			constexpr float EPS = 0.0001f;
			const bool isCloser = distSq < bestDistSq - EPS;
			const bool isSameDist = std::fabsf(distSq - bestDistSq) <= EPS;

			if (isCloser || (isSameDist && centerDistSq < bestCenterDistSq))
			{
				bestColumn = c;
				bestRow = r;
				bestDistSq = distSq;
				bestCenterDistSq = centerDistSq;
			}
		}
	}

	// -- 3. 見た目を変えずに、Playerの原点を回転軸Pieceの位置へ移す -- //
	if (auto transform = GetTransform())
	{
		transform->SetPosition(PlayerMath::LocalToWorld(
			transform->GetPosition(),
			transform->GetRotation().z,
			mPieces[bestColumn][bestRow]->GetLocalPosition()));
	}

	// -- 4. 全Pieceの相対座標を「回転軸Pieceからの距離」に置き直す -- //
	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			if (mPieces[c][r] == nullptr)
				continue;

			mPieces[c][r]->SetLocalPosition(VGet(
				static_cast<float>(c - bestColumn) * GameConfig::CELL_SIZE,
				static_cast<float>(r - bestRow) * GameConfig::CELL_SIZE,
				0.0f
			));
		}
	}

	mnPivotColumn = bestColumn;
	mnPivotRow = bestRow;
}
