#pragma once
#include "DxLib.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h> //<--フォント追加のAPI（AddFontResourceEXなど） 
class FontManager {
public:
    // コンストラクタとデストラクタを public に配置する
    FontManager() = default;
    ~FontManager() { Clear(); }

    // コピー禁止（意図しないコピー防止）
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

	//-- フォントの取得と解放--//  ->thickのデフォルト値を-1に設定することで、太さを指定しない場合はデフォルトの太さが使用されるようにする
    int GetFont(const std::string& fontName, int size, int thick = -1, int fontType = DX_FONTTYPE_ANTIALIASING);
	//-- フォントの破棄 --//
    void ReleaseFont(const std::string& fontName, int size, int thick = -1, int fontType = DX_FONTTYPE_ANTIALIASING);
	//-- フォントの解放 --//
    void Clear();
	//-- フォントファイルの読み込み --//
    bool LoadFontFile(const std::string& filePath);

private:
    // フォントのパラメータ（名前・サイズ・太さ・描画タイプ）から一意の検索キー文字列を生成
    std::string GenerateKey(const std::string& fontName, int size, int thick, int fontType) const {
        return fontName + "_" + std::to_string(size) + "_" + std::to_string(thick) + "_" + std::to_string(fontType);
    }
    // 生成済みフォントのキャッシュテーブル（Key: パラメータ文字列, Value: DXライブラリのフォントハンドル）
    std::unordered_map<std::string, int> fontMap_;
    // Windows API等で読み込んだフォントファイルのリソースハンドル保持リスト（終了時の一括解放用）
    std::vector<HANDLE> loadedFontFiles_;

};