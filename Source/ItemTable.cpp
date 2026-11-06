// ItemTable.cpp
#include "ItemTable.h"

namespace {
	constexpr const char* ITEM_DIR = "Resource/Collectible/Stage";

	constexpr ItemDef ITEM_DEFS[] = {
		{1,0,"Negi.png"},    {1,1,"Syouga.png"},     {1,2,"Syouyu.png"},
		{2,0,"Konbu.png"},   {2,1,"Negi.png"},       {2,2,"Ponzu.png"},
		{3,0,"Oil.png"},     {3,1,"Salt.png"},       {3,2,"Water.png"},
		{4,0,"Miso.png"},    {4,1,"Negi.png"},       {4,2,"Wakame.png"},
		{5,0,"Garlic.png"},  {5,1,"Hikiniku.png"},   {5,2,"Toubanjan.png"},
		{6,0,"Gyuniku.png"}, {6,1,"Negi.png"},       {6,2,"Tare.png"},
		{7,0,"Mentuyu.png"}, {7,1,"Negi.png"},       {7,2,"Oil.png"},
		{8,0,"Kukonomi.png"},{8,1,"Kyouninsou.png"}, {8,2,"Milk.png"},
	};
}

const ItemDef* ItemTable::Find(int _stage, int _slot) {
	for (const auto& def : ITEM_DEFS) {
		if (def.stage == _stage && def.slot == _slot)
			return &def;
	}
	return nullptr;
}

std::string ItemTable::GetFilePath(int _stage, int _slot) {
	const ItemDef* def = Find(_stage, _slot);
	if (def == nullptr)
		return "";
	return std::string(ITEM_DIR) + std::to_string(def->stage) + "/" + def->filename;
}