#include "Player.h"

/* memo :
* Player の「形状」部分。
*
* ■ 2種類の向き
*   Player基準 : mPieces[column][row] の並び。Playerが回転しても変わらない
*   画面基準   : 変形UIに表示する向き。Playerの回転(90度単位)を打ち消したもの
*   変形UIとのやり取りは画面基準、内部の処理は Player基準 で行う。
*
* ■ 形状変更の流れ (TryChangeShapeFromScreen)
*   画面基準 → Player基準 に変換
*   → 基盤ブロックが残っているか → ひとつの塊か → 地形にめり込まないか
*   → 適用(回転軸も選び直す)
*/

// -- Module -- //
#include "Transform.h"

#include "PlayerPiece.h"
#include "GameConfig.h"

#include <DxLib.h>
#include <cmath>
#include <vector>
#include <utility>

namespace
{
	constexpr int N = GameConfig::PLAYER_PIECE_SIZE;
	constexpr int SHAPE_CENTER = N / 2;	// 中央マス(1)

	// 同じ高さとみなす誤差(基盤ブロック選択用)
	constexpr float BASE_SAME_HEIGHT_EPS = GameConfig::CELL_SIZE * 0.25f;

	// -- プリセット形状 ('#' = ブロックあり / 上の行から順に) -- //
	constexpr const char* PRESET_SHAPES[][N] = {
		{ ".#.",
		  "###",
		  "..." },	// 0 : T字

		{ ".#.",
		  "###",
		  ".#." },	// 1 : 十字

		{ "###",
		  "###",
		  "###" },	// 2 : 3×3

		{ "##.",
		  "##.",
		  "..." },	// 3 : 2×2
	};
	constexpr int PRESET_SHAPE_COUNT = sizeof(PRESET_SHAPES) / sizeof(PRESET_SHAPES[0]);

	PlayerShapeMask MakePresetShape(int _shapeType)
	{
		PlayerShapeMask shape{};

		for (int r = 0; r < N; ++r)
		{
			for (int c = 0; c < N; ++c)
			{
				// 範囲外の番号は 3×3 全部
				shape[c][r] = (_shapeType < 0 || _shapeType >= PRESET_SHAPE_COUNT)
					? true
					: PRESET_SHAPES[_shapeType][r][c] == '#';
			}
		}

		return shape;
	}

	// マス(_column, _row)を中央マス基準で 90度 × _quarterTurns 回転させる
	// UpdatePieceTransforms と同じ回転方向 : (dx, dy) → (-dy, dx)
	void RotateCell(int& _column, int& _row, int _quarterTurns)
	{
		int dx = _column - SHAPE_CENTER;
		int dy = _row - SHAPE_CENTER;

		for (int i = 0; i < _quarterTurns; ++i)
		{
			const int prevDx = dx;
			dx = -dy;
			dy = prevDx;
		}

		_column = dx + SHAPE_CENTER;
		_row = dy + SHAPE_CENTER;
	}

	// 形状全体を 90度 × _quarterTurns 回転させる
	PlayerShapeMask RotateShape(const PlayerShapeMask& _shape, int _quarterTurns)
	{
		PlayerShapeMask rotated{};

		for (int r = 0; r < N; ++r)
		{
			for (int c = 0; c < N; ++c)
			{
				if (!_shape[c][r])
					continue;

				int column = c;
				int row = r;
				RotateCell(column, row, _quarterTurns);
				rotated[column][row] = true;
			}
		}

		return rotated;
	}
}

// ============================================================ //
//  形状の取得・設定
// ============================================================ //
void Player::SetShape(int _shapeType)
{
	ApplyLocalShape(MakePresetShape(_shapeType));
}

PlayerShapeMask Player::GetLocalShape() const
{
	PlayerShapeMask shape{};

	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			shape[c][r] = (mPieces[c][r] != nullptr && mPieces[c][r]->IsActive());
		}
	}

	return shape;
}

void Player::ApplyLocalShape(const PlayerShapeMask& _shape)
{
	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			if (mPieces[c][r] != nullptr)
				mPieces[c][r]->SetActive(_shape[c][r]);
		}
	}

	// 形が変わったので回転軸を選び直す(見た目の位置は変わらない)
	RecalculateRotationPivot();
}

int Player::GetQuarterTurns() const
{
	auto transform = GetTransform();
	if (!transform)
		return 0;

	// 一番近い90度単位に丸める
	int turns = static_cast<int>(std::lroundf(transform->GetRotation().z / (DX_PI_F / 2.0f)));

	turns %= 4;
	if (turns < 0)
		turns += 4;

	return turns;
}

// ============================================================ //
//  変形UI とのやり取り (画面基準)
// ============================================================ //
void Player::BeginShapeEdit()
{
	if (mbIsShapeEditing)
		return;

	mbIsShapeEditing = true;
	mbIsToppling = false;

	UpdatePieceTransforms();
	SelectBaseCell();
}

void Player::EndShapeEdit()
{
	mbIsShapeEditing = false;
}

PlayerShapeMask Player::GetScreenShape() const
{
	// Player基準 → 画面基準
	return RotateShape(GetLocalShape(), GetQuarterTurns());
}

void Player::GetScreenBaseCell(int& _column, int& _row) const
{
	_column = mnBaseColumn;
	_row = mnBaseRow;
	RotateCell(_column, _row, GetQuarterTurns());
}

ShapeChangeResult Player::TryChangeShapeFromScreen(const PlayerShapeMask& _screenShape)
{
	// -- 1. 画面基準 → Player基準 (回転を逆向きにかける) -- //
	const PlayerShapeMask local = RotateShape(_screenShape, (4 - GetQuarterTurns()) % 4);

	// -- 2. 基盤ブロックが残っているか -- //
	if (!local[mnBaseColumn][mnBaseRow])
		return ShapeChangeResult::BaseRemoved;

	// -- 3. ひとつの塊になっているか -- //
	if (!IsConnected(local, mnBaseColumn, mnBaseRow))
		return ShapeChangeResult::Disconnected;

	// -- 4. 今の位置・回転のまま地形にめり込まないか -- //
	auto transform = GetTransform();
	if (!transform)
		return ShapeChangeResult::Overlapping;

	const PoseCollisionResult collision =
		TestCollisionAtPose(transform->GetPosition(), transform->GetRotation().z, local);

	if (!IsPoseAcceptable(collision))
		return ShapeChangeResult::Overlapping;

	// -- 5. 適用 (各Pieceのワールド位置は変わらないので、基盤ブロックも動かない) -- //
	ApplyLocalShape(local);
	UpdatePieceTransforms();
	return ShapeChangeResult::Success;
}

// ============================================================ //
//  形状変更の内部処理
// ============================================================ //
void Player::SelectBaseCell()
{
	// 画面上で一番下にあるブロック(=地面に一番近いブロック)を基盤にする。
	// 同じ高さが複数ある場合は、重心に一番近いものを選ぶ。
	const VECTOR com = CalculateCenterOfMass();

	bool found = false;
	float bestY = 0.0f;
	float bestDistX = 0.0f;

	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			auto piece = mPieces[c][r];
			if (!piece || !piece->IsActive())
				continue;

			auto pieceTransform = piece->GetModule<Transform>();
			if (!pieceTransform)
				continue;

			const VECTOR pos = pieceTransform->GetPosition();
			const float distX = std::fabsf(pos.x - com.x);

			const bool isLower = pos.y > bestY + BASE_SAME_HEIGHT_EPS;	// 画面座標はyが下向き
			const bool isSameHeight = std::fabsf(pos.y - bestY) <= BASE_SAME_HEIGHT_EPS;

			if (!found || isLower || (isSameHeight && distX < bestDistX))
			{
				found = true;
				bestY = pos.y;
				bestDistX = distX;
				mnBaseColumn = c;
				mnBaseRow = r;
			}
		}
	}
}

bool Player::IsConnected(const PlayerShapeMask& _shape, int _startColumn, int _startRow)
{
	constexpr int DIRECTIONS[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

	if (!_shape[_startColumn][_startRow])
		return false;

	// 基盤ブロックから上下左右にたどれるブロックを数える(深さ優先探索)
	PlayerShapeMask visited{};
	std::vector<std::pair<int, int>> stack;

	stack.push_back({ _startColumn, _startRow });
	visited[_startColumn][_startRow] = true;
	int reachedCount = 0;

	while (!stack.empty())
	{
		const std::pair<int, int> cell = stack.back();
		stack.pop_back();
		++reachedCount;

		for (const auto& dir : DIRECTIONS)
		{
			const int nextColumn = cell.first + dir[0];
			const int nextRow = cell.second + dir[1];

			if (nextColumn < 0 || nextColumn >= N || nextRow < 0 || nextRow >= N)
				continue;
			if (!_shape[nextColumn][nextRow] || visited[nextColumn][nextRow])
				continue;

			visited[nextColumn][nextRow] = true;
			stack.push_back({ nextColumn, nextRow });
		}
	}

	// 全ブロック数と一致すれば「ひとつの塊」
	int totalCount = 0;
	for (int r = 0; r < N; ++r)
	{
		for (int c = 0; c < N; ++c)
		{
			if (_shape[c][r])
				++totalCount;
		}
	}

	return reachedCount == totalCount;
}
