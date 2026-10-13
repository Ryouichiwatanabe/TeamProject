#pragma once
#include "Module.h"

class GameObject;

class Gravity : public Module
{
public:
	void Update(float _deltaTime) override;

	// -- 接地判定のアクセサ -- //
	void SetGrounded(bool _grounded) { mbIsGrounded = _grounded; }
	bool IsGrounded() { return mbIsGrounded; }

	float GetVelocityY() { return mfVelocityY; }	// (Debag用) 落下速度の取得

private:
	void ResetVerticalVelocity();	// 落下速度を「0」にする

private:
	float mfVelocityY = 0.0f;	// 落下速度

	bool mbIsGrounded = false;	// 接地判定を示す
};
