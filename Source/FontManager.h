#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include <unordered_map>

enum class FontId
{
    Default,    // フォールバック用(必ず登録する)
    Title,
    SubTitle,
    Button,
};

struct FontDesc
{
    std::string name;                           // フォント名(ファイル名ではない)
    int size = 24;                              // 大きさ
    int thick = -1;                             // 太さ(-1でデフォルト)
    int fontType = DX_FONTTYPE_ANTIALIASING;    // 縁付きなら _EDGE 系
    int edgeSize = -1;                          // 縁のサイズ
};

/* memo :
* フォントハンドルをFontIdで管理するクラス。
*
* // -- 使い方 -- //
*	1. (自前の.ttfを使う場合) LoadFontFile()でフォントファイルを読み込む
*	2. Register()でFontIdとフォント設定を登録する(起動時にまとめて行う)
*	   ※ FontId::Defaultは必ず登録する
*	3. GetHandle()でハンドルを取得して描画に使う
*	4. 終了時、DxLib_End()より前にClear()を呼ぶ
*
* // -- 注意 -- //
*	ハンドルは共有されるので、使う側でDeleteFontToHandle()を呼ばないこと。
*/

class FontManager {
public:
	FontManager() = default;
	~FontManager();

    bool LoadFontFile(const std::string& _filePath);     // ① .ttf の読み込み
    bool Register(FontId _id, const FontDesc& _desc);    // ハンドル生成 + ID登録
    int  GetHandle(FontId _id) const;                     // 取得(なければDefault)
    void Clear();                                         // 全解放

private:
    std::unordered_map<FontId, int> mHandles;				// FontId → フォントハンドル
    std::unordered_map<std::string, HANDLE> mFontFiles;		// ファイルパス → フォントファイルのハンドル
};