#pragma once

struct ItemDef {
	int stage, slot;
	const char* itemFilename;
};

class ItemTable {
public:
	// -- Todo : csvを読み込む処理 -- //

	void Release();	// 画像の解放

	// -- IDからアイテム情報を取得 -- //
	const ItemDef* GetItemDef(int _stage, int _slot) const;

	// -- IDをステージ番号,スロット番号から取得する -- //
		// Todo : 不要であるなら消す
	// const int ToItemId(int _stage, int _slot) const;
private:
};