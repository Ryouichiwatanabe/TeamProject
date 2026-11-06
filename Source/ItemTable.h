#pragma once
#include <string>

struct ItemDef {
	int stage;
	int slot;
	const char* filename;
};

namespace ItemTable {
	const ItemDef* Find(int _stage, int _slot);			// 無ければ nullptr
	std::string GetFilePath(int _stage, int _slot);		// 無ければ ""
}