#pragma once

class ItemTable {
public:
	// -- Todo : csvを読み込む処理 -- //

	void Release();	// 画像の解放

	// -- IDからアイテム情報を取得 -- //
	const ItemDef* GetItemDef(int _stage, int _slot) const;

	// -- IDをステージ番号,スロット番号から取得する -- //
	const int ToItemId(int _stage, int _slot) const;
private:
};