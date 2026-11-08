#pragma once
#include <string>

struct ItemDef {
	int stage;
	int slot;
	const char* filename;
};

struct DishDef {
	int stage;
	const char* filename;
	const char* name;		// 料理名(表示用)
};

namespace ItemTable {
	const ItemDef* Find(int _stage, int _slot);			// 無ければ nullptr
	std::string GetFilePath(int _stage, int _slot);		// 無ければ ""

	// -- 料理 -- //
	const DishDef* FindDish(int _stage);				// 無ければ nullptr
	std::string GetDishFilePath(int _stage);			// 無ければ ""
	std::string GetDishName(int _stage);				// 無ければ ""
}