#include "Player.h"

/* memo :
* Player の「衝突・接地」部分。
*
* ■ 押し戻し
*   各Pieceが報告した押し戻し量を1つにまとめて Player を動かす。
*   (Pieceごとに動かすとバラバラになるため、ハブの Player で一括処理)
*
* ■ 接地・倒れ込み
*   下から支えている接触(床)を集めて判断する。
*     床に触れていない       → 空中
*     重心が支えの範囲内     → 安定して接地
*     重心が支えの範囲外     → 支点まわりに少し倒す(毎フレーム繰り返して倒れ込む)
*/

// -- Module -- //
#include "Transform.h"
#include "Gravity.h"

#include "GameConfig.h"
#include "PlayerMath.h"

#include "SoundManager.h"

#include <DxLib.h>
#include <cmath>
#include <cfloat>

namespace
{
	constexpr float RIGHT_ANGLE = DX_PI_F / 2.0f;		// 90度
	constexpr float NEARLY_FLAT_TOLERANCE = 0.08f;		// 90度単位から何ラジアン以内なら「平ら」とみなすか
	constexpr float MULTI_SUPPORT_WIDTH = GameConfig::CELL_SIZE * 0.7f;	// これ以上離れた2点で支えていれば安定

	// 角度が 90度単位(0, 90, 180, 270度)にほぼ一致しているか
	bool IsNearlyFlat(float _rotZ)
	{
		float modAngle = std::fmodf(std::fabsf(_rotZ), RIGHT_ANGLE);
		if (modAngle < 0.0f)
			modAngle += RIGHT_ANGLE;

		return modAngle < NEARLY_FLAT_TOLERANCE || modAngle > (RIGHT_ANGLE - NEARLY_FLAT_TOLERANCE);
	}

	// 支えている接触のうち、重心に一番近いものの位置(=倒れ込みの支点)
	VECTOR FindClosestContactPosition(const std::vector<PieceContact>& _contacts, const VECTOR& _point)
	{
		VECTOR closest = _contacts[0].mvPieceWorldPos;
		float closestDistSq = FLT_MAX;

		for (const auto& contact : _contacts)
		{
			const float dx = contact.mvPieceWorldPos.x - _point.x;
			const float dy = contact.mvPieceWorldPos.y - _point.y;
			const float distSq = dx * dx + dy * dy;

			if (distSq < closestDistSq)
			{
				closestDistSq = distSq;
				closest = contact.mvPieceWorldPos;
			}
		}

		return closest;
	}
}

// ============================================================ //
//  衝突処理の前後
// ============================================================ //
void Player::BeginCollisionResolution()
{
	GameObject::BeginCollisionResolution();

	mCollisionCorrections.clear();
	mPieceContacts.clear();
}

void Player::EndCollisionResolution()
{
	GameObject::EndCollisionResolution();

	// 変形UIを開いている間は位置・回転を一切動かさない
	if (mbIsShapeEditing)
	{
		UpdatePieceTransforms();
		return;
	}

	ApplyCollisionCorrection();		// 1. めり込みを押し戻す
	UpdatePieceTransforms();

	ProcessGroundAndTopple();		// 2. 接地 / 倒れ込み
	UpdatePieceTransforms();
}

// ============================================================ //
//  PlayerPiece からの報告
// ============================================================ //
void Player::AddCollisionCorrection(const VECTOR& _correction)
{
	if (_correction.x == 0.0f && _correction.y == 0.0f)
		return;

	mCollisionCorrections.push_back(_correction);
}

void Player::AddPieceContact(const PieceContact& _contact)
{
	mPieceContacts.push_back(_contact);
}

// ============================================================ //
//  押し戻し
// ============================================================ //
VECTOR Player::CalculateCollisionCorrection() const
{
	// 各方向(+X, -X, +Y, -Y)で一番大きい押し戻しを採用して合計する
	// (同じ方向の押し戻しを足し合わせると押し戻しすぎるため)
	float minX = 0.0f, maxX = 0.0f;
	float minY = 0.0f, maxY = 0.0f;

	for (const VECTOR& candidate : mCollisionCorrections)
	{
		if (candidate.x > maxX) maxX = candidate.x;
		if (candidate.x < minX) minX = candidate.x;
		if (candidate.y > maxY) maxY = candidate.y;
		if (candidate.y < minY) minY = candidate.y;
	}

	return VGet(maxX + minX, maxY + minY, 0.0f);
}

void Player::ApplyCollisionCorrection()
{
	auto transform = GetTransform();
	if (!transform)
		return;

	VECTOR correction = CalculateCollisionCorrection();

	// 不感帯 : ごく小さい押し戻しは無視(細かい振動を防ぐ)
	if (std::fabsf(correction.x) < GameConfig::PENETRATION_DEADZONE)
		correction.x = 0.0f;
	if (std::fabsf(correction.y) < GameConfig::PENETRATION_DEADZONE)
		correction.y = 0.0f;

	if (correction.x == 0.0f && correction.y == 0.0f)
		return;

	// 減衰 : 押し戻しすぎ(行き過ぎて逆にめり込む)を防ぐ
	correction = VScale(correction, GameConfig::COLLISION_DAMPING);

	transform->SetPosition(VAdd(transform->GetPosition(), correction));
}

// ============================================================ //
//  接地・倒れ込み
// ============================================================ //
void Player::ProcessGroundAndTopple()
{
	const std::vector<PieceContact> groundContacts = CollectGroundContacts();

	// -- 空中 -- //
	if (groundContacts.empty())
	{
		SetGroundedState(false);
		mbIsToppling = false;

		return;
	}

	if (!GetTransform())
		return;

	// -- 安定して接地 -- //
	if (IsStableOnGround(groundContacts))
	{
		SetGroundedState(true);
		mbIsToppling = false;
		SnapRotationIfNearlyFlat();
		return;
	}

	// -- 不安定(重心が支えの外) -- //
	SetGroundedState(false);

	// 回転入力の直後は倒れ込みを止める(入力と倒れ込みが打ち消し合わないように)
	if (mfInputCooldown > 0.0f)
	{
		mbIsToppling = false;
		return;
	}

	Topple(groundContacts);
}

std::vector<PieceContact> Player::CollectGroundContacts() const
{
	std::vector<PieceContact> groundContacts;

	for (const auto& contact : mPieceContacts)
	{
		// 上向きの押し戻し = 下から支えられている
		if (contact.mCollisionInfo.normal.y < -0.5f)
			groundContacts.push_back(contact);
	}

	return groundContacts;
}

bool Player::IsStableOnGround(const std::vector<PieceContact>& _groundContacts) const
{
	// 支えている範囲(x方向)
	float minSupportX = FLT_MAX;
	float maxSupportX = -FLT_MAX;
	for (const auto& contact : _groundContacts)
	{
		const float pieceX = contact.mvPieceWorldPos.x;
		if (pieceX < minSupportX) minSupportX = pieceX;
		if (pieceX > maxSupportX) maxSupportX = pieceX;
	}

	// 重心が支えの範囲内か
	const VECTOR com = CalculateCenterOfMass();
	const bool isComInsideSupport =
		(com.x >= minSupportX - GameConfig::STABLE_GROUND_THRESHOLD_X) &&
		(com.x <= maxSupportX + GameConfig::STABLE_GROUND_THRESHOLD_X);

	if (!isComInsideSupport)
		return false;

	// 平らに置かれている or 離れた2点以上で支えている
	const bool isNearlyFlat = IsNearlyFlat(GetTransform()->GetRotation().z);
	const bool isMultiPointSupport = (maxSupportX - minSupportX) >= MULTI_SUPPORT_WIDTH;

	return isNearlyFlat || isMultiPointSupport;
}

void Player::SnapRotationIfNearlyFlat()
{
	auto transform = GetTransform();
	const float rotZ = transform->GetRotation().z;

	if (!IsNearlyFlat(rotZ))
		return;

	const float snappedRotZ = std::roundf(rotZ / RIGHT_ANGLE) * RIGHT_ANGLE;
	transform->SetRotation(VGet(0.0f, 0.0f, snappedRotZ));
}

void Player::Topple(const std::vector<PieceContact>& _groundContacts)
{
	auto transform = GetTransform();
	const float rotZ = transform->GetRotation().z;

	// -- 1. 支点 : 重心に一番近い接地Piece -- //
	const VECTOR com = CalculateCenterOfMass();
	const VECTOR pivot = FindClosestContactPosition(_groundContacts, com);

	// -- 2. 倒れる向き : 重心が支点より右なら時計回り(+)、左なら反時計回り(-) -- //
	float direction = 0.0f;
	if (com.x > pivot.x + GameConfig::STABLE_GROUND_THRESHOLD_X)
		direction = 1.0f;
	else if (com.x < pivot.x - GameConfig::STABLE_GROUND_THRESHOLD_X)
		direction = -1.0f;
	else
		direction = (std::sinf(rotZ) >= 0.0f) ? 1.0f : -1.0f;	// ほぼ真上 : 今傾いている向きへ

	// -- 3. 支点まわりに少しだけ回す -- //
	const float step = direction * PlayerMath::DegToRad(GameConfig::TOPPLE_SPEED_DEG);
	const VECTOR offsetFromPivot = VSub(transform->GetPosition(), pivot);

	transform->SetPosition(VAdd(pivot, PlayerMath::RotateOffset(offsetFromPivot, step)));
	transform->SetRotation(VGet(0.0f, 0.0f, rotZ + step));

	mbIsToppling = true;
}

void Player::SetGroundedState(bool _grounded)
{
	mbIsGrounded = _grounded;

	if (auto gravity = GetModule<Gravity>())
		gravity->SetGrounded(_grounded);
}
