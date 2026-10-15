#include "Gravity.h"

#include <DxLib.h>
#include <algorithm>

#include "GameObject.h"
#include "Transform.h"

#include "ScreenConfig.h"
#include "GameConfig.h"

void Gravity::Update(float _deltaTime)
{
	if (!mbIsGrounded)
	{
		ResetVerticalVelocity();
		return;
	}

	// 重力加速度によって落下速度を計算
	mfVelocityY += GameConfig::GLAVITY * _deltaTime;

	// -- 座標を取得 -- //
	auto transform = mpGameObject->GetModule<Transform>();
	VECTOR position = transform->GetPosition();

	mfVelocityY = std::min<float>(mfVelocityY, GameConfig::MAX_FALLSPEED);

	// 2D座標ではYが下方向なので加算
	position.y += mfVelocityY;

	// y = 0 を地面として、それより下に行かないようにする
	if (position.y >= ScreenConfig::SCREEN_HEIGHT)
	{
		position.y = ScreenConfig::SCREEN_HEIGHT;
		mfVelocityY = 0.0f;
	}

	// Objに座標を反映
	transform->SetPosition(position);
}

void Gravity::ResetVerticalVelocity()
{
	mfVelocityY = 0.0f;
}