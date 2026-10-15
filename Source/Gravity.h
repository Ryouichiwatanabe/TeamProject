#pragma once
#include "Module.h"

class GameObject;

class Gravity : public Module
{
public:
	void Update(float _deltaTime);

	// -- 接地判定のアクセサ -- //
	void SetGrounded(bool _grounded) { mbIsGrounded = _grounded; }
	bool IsGrounded() { return mbIsGrounded; }

private:
	void ResetVerticalVelocity();	// 落下速度を「0」にする

private:
	float mfVelocityY = 0.0f;	// 落下速度

	bool mbIsGrounded = true;	// 接地判定を示す
};
