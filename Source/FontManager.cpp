#include "FontManager.h"


int FontManager::GetFont(const std::string& fontName, int size, int thick, int fontType) {
    std::string key = GenerateKey(fontName, size, thick, fontType);

    // すでにキャッシュにある場合は既存のハンドルを返す
    auto it = fontMap_.find(key);
    if (it != fontMap_.end()) {
		return it->second; //既存のフォントハンドルを返す
    }

    // キャッシュにない場合は新規作成
    int handle = CreateFontToHandle(
        fontName.empty() ? nullptr : fontName.c_str(),
        size,
        thick,
        fontType
    );

	//生成成功時のみキャッシュに登録
    if (handle != -1) {
        fontMap_[key] = handle;
    }

    return handle;
}

//個別フォントハンドルの破棄
void FontManager::ReleaseFont(const std::string& fontName, int size, int thick, int fontType) {
    std::string key = GenerateKey(fontName, size, thick, fontType);
    auto it = fontMap_.find(key);
    if (it != fontMap_.end()) {
        DeleteFontToHandle(it->second);
        fontMap_.erase(it);
    }
}

// フォントファイルの読み込み
bool FontManager::LoadFontFile(const std::string& filePath)
{

	HANDLE fontHandle = AddFontFile(filePath.c_str());
    if (fontHandle != NULL)
    {
        loadedFontFiles_.push_back(fontHandle);
		return true;
    }
	return false;
}


void FontManager::Clear() 
{
	//フォントハンドルをすべて削除

    for (auto& pair : fontMap_) 
    {
        DeleteFontToHandle(pair.second);
    }
    fontMap_.clear();

	//読み込んだフォントファイルのリストをクリア
    for (const auto& handle : loadedFontFiles_) 
    {
        RemoveFontFile(handle);
	}
	loadedFontFiles_.clear();
}

