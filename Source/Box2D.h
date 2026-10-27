#pragma once
#include <DxLib.h>	// VECTOR
#include <cmath>	// sinf,conf

// 衝突処理に適したデータ構造型
struct Box2D {
	VECTOR position;	// 中心座標
	VECTOR half;		// 半サイズ
	VECTOR axisX;		// ローカル軸X
	VECTOR axisY;		// ローカル軸Y

	// 回転角(ラジアン)から軸を作るファクトリ関数
	static Box2D Make(const VECTOR& _position, const VECTOR& _halfSize, float _rotationZ) {
		const float cosValue = cosf(_rotationZ);
		const float sinValue = sinf(_rotationZ);

		return {
			_position,
			_halfSize,
			VGet(cosValue, sinValue, 0.0f),
			VGet(-sinValue, cosValue, 0.0f)
		};
	}

	// 4頂点の取得
	void GetVertices(VECTOR _outVertices[4]) const {
		VECTOR extX = VScale(axisX, half.x);
		VECTOR extY = VScale(axisY, half.y);
		_outVertices[0] = VAdd(VAdd(position, extX), extY);
		_outVertices[1] = VSub(VAdd(position, extX), extY);
		_outVertices[2] = VSub(VSub(position, extX), extY);
		_outVertices[3] = VAdd(VSub(position, extX), extY);
	}

	// 単位ベクトル軸に対する半投影幅（半径）を計算
	float ProjectToAxis(const VECTOR& _axis) const {
		return std::fabsf(axisX.x * _axis.x + axisX.y * _axis.y) * half.x +
		       std::fabsf(axisY.x * _axis.x + axisY.y * _axis.y) * half.y;
	}
};