#pragma once
#pragma once
#include <span>
#include <string>

// 取得スロットのビット（slot0 = 1, slot1 = 2, slot2 = 4）
namespace BgBit {
	constexpr int S0 = 1 << 0;
	constexpr int S1 = 1 << 1;
	constexpr int S2 = 1 << 2;
	constexpr int ALL = S0 | S1 | S2;
}

// 背景に重ねる画像1枚分の定義
struct BgLayerDef {
	const char* file;	// 料理フォルダ内のファイル名
	int require;		// このビットが「全部」取得済みなら描く
	int exclude;		// このビットが「1つでも」取得済みなら描かない
	float x, y;			// 中心座標（Foundation画像のピクセル座標）
	float scale;		// 拡大率（Foundation画像基準）
	float angle;		// 回転（ラジアン）
};

// 料理1品分の定義
struct BgDishDef {
	int stage;
	const char* folder;					// Resource/UI/BackGround/ 以下のフォルダ名
	const char* foundation;				// 土台画像（画面いっぱいに描く）
	std::span<const BgLayerDef> layers;	// 描画順（上に書いたものほど奥）
};

namespace StageBackGroundTable {
	const BgDishDef* Find(int _stage);										// 無ければ nullptr
	std::string GetFilePath(const BgDishDef& _dish, const char* _file);
}