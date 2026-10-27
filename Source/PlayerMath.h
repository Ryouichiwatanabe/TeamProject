#pragma once
#include <DxLib.h>
#include <cmath>

/* memo :
* Player まわりで何度も出てくる計算をまとめた小さな関数群。
* 画面座標は y が下向きなので、角度がプラス = 時計回り。
*/
namespace PlayerMath
{
	// 度 → ラジアン
	inline float DegToRad(float _degree)
	{
		return _degree * DX_PI_F / 180.0f;
	}

	// ベクトル(相対座標)を原点まわりに _rotZ だけ回転させる
	inline VECTOR RotateOffset(const VECTOR& _offset, float _rotZ)
	{
		const float cosVal = std::cosf(_rotZ);
		const float sinVal = std::sinf(_rotZ);

		return VGet(
			cosVal * _offset.x - sinVal * _offset.y,
			sinVal * _offset.x + cosVal * _offset.y,
			0.0f
		);
	}

	// 「原点 _origin・角度 _rotZ」の姿勢で、相対座標 _localPos がワールドのどこに来るか
	inline VECTOR LocalToWorld(const VECTOR& _origin, float _rotZ, const VECTOR& _localPos)
	{
		const VECTOR rotated = RotateOffset(_localPos, _rotZ);
		return VGet(_origin.x + rotated.x, _origin.y + rotated.y, 0.0f);
	}
}
