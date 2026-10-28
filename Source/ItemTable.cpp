#include "ItemTable.h"



#include "Master.h"
#include "SceneManager.h"

namespace {
	struct ItemDef {
		int stage, slot;
		const char* itemFilename;
	};

	// -- アイテムのIDとファイルネームを登録 -- //
	constexpr ItemDef ItemDefs[] = {
			{1,0,"Negi.png"},
			{1,1,"Syouga.png"},
			{1,2,"Syouyu.png"},
			{2,0,"Konbu.png"},
			{2,1,"Negi.png"},
			{2,2,"Ponzu.png"},
	};
}

// -- Todo : ItemDefsを解放 -- //
void ItemTable::Release() {

}

const ItemDef* ItemTable::GetItemDef(int _stage, int _slot) const {
	for (auto& def : ItemDefs)
		if (def.stage == _stage
			&& def.slot == _slot)
			return &def;
	
	return nullptr;
}



