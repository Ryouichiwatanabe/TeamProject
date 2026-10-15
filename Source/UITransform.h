#pragma once
#include <DxLib.h>

/* memo : 
* UIに用いる空間上の状態を管理するModule
*/

class UITransform
{
public:
	UITransform() = default;
	~UITransform() = default;

	// -- 座標値アクセサ -- //
	void SetPosition(VECTOR _position) { mvPosition = _position; }
	VECTOR GetPosition() { return mvPosition; }

	// -- サイズ値アクセサ -- //
	void SetSize(VECTOR _size) { mvSize = _size; }
	VECTOR GetSize() { return mvSize; }

	// -- 拡縮値アクセサ -- //
	void SetScale(VECTOR _scale) { mvScale = _scale; }
	VECTOR GetScale() { return mvScale; }

	// -- 回転値アクセサ -- //
	void SetRotation(VECTOR _rotation) { mvRotation = _rotation; }
	VECTOR GetRotation() { return mvRotation; }

	// -- アンカー座標値アクセサ -- //
	void SetAnchor(VECTOR _anchor) { mvAnchor = _anchor; }
	VECTOR GetAnchor() { return mvAnchor; }

	void SetPivot(VECTOR _pivot) { mvPivot = _pivot; }
	VECTOR GetPivot() { return mvPivot; }

	VECTOR GetCenterPosition();	// 
	VECTOR GetPivotLocalPosition();

private:
	VECTOR mvPosition = VGet(0.0f, 0.0f, 0.0f);	// 座標値
	VECTOR mvSize = VGet(0.0f, 0.0f, 0.0f);		// 大きさ
	VECTOR mvScale = VGet(1.0f, 1.0f, 0.0f);	// 拡縮値
	VECTOR mvRotation = VGet(0.0f, 0.0f, 0.0f);	// 回転値(主にzを用いる)

	VECTOR mvAnchor = VGet(0.5f, 0.5f, 0.0f);	// 画面上の正規値(左上(0.0f,0.0f,0.0f) / 右下(1.0f,1.0f,0.0f))
	VECTOR mvPivot = VGet(0.5f, 0.5f, 0.0f);	// UI内の中の座標を比率で表す値(正規値)
};