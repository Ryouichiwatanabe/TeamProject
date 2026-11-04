#include "FontManager.h"

FontManager::~FontManager()
{
	// DxLib_End()後に解放処理を呼ばないためのガード
	if (DxLib_IsInit())
	{
		Clear();
	}
}

bool FontManager::LoadFontFile(const std::string& _filePath)
{
	// -- 読み込み済みなら何もしない -- //
	if (mFontFiles.find(_filePath) != mFontFiles.end())
		return true;

	// -- フォントファイルを読み込む -- //
	HANDLE fontFile = AddFontFile(_filePath.c_str());
	if (fontFile == NULL) {
		// Todo : Debug.hのログで通知
		return false;
	}

	mFontFiles[_filePath] = fontFile;
	return true;
}

bool FontManager::Register(FontId _id, const FontDesc& _desc)
{
	// -- 同じIDが登録済みなら、古いハンドルを先に解放 -- //
	auto it = mHandles.find(_id);
	if (it != mHandles.end())
	{
		DeleteFontToHandle(it->second);
		mHandles.erase(it);
	}

	// -- ハンドルを生成 -- //
	int handle = CreateFontToHandle(
		_desc.name.empty() ? nullptr : _desc.name.c_str(),	// 空ならDxLibのデフォルトフォント
		_desc.size,
		_desc.thick,
		_desc.fontType,
		-1,							// 文字セット(デフォルト)
		_desc.edgeSize
	);
	if (handle == -1)
	{
		// Todo : Debug.hのログで通知
		return false;
	}

	mHandles[_id] = handle;
	return true;
}

int FontManager::GetHandle(FontId _id) const
{
	// -- 指定IDを探す -- //
	auto it = mHandles.find(_id);
	if (it != mHandles.end())
		return it->second;

	// -- 見つからなければDefaultを探す -- //
	auto defaultIt = mHandles.find(FontId::Default);
	if (defaultIt != mHandles.end())
		return defaultIt->second;

	// -- Defaultも無ければDxLib標準フォント -- //
	return DX_DEFAULT_FONT_HANDLE;
}

void FontManager::Clear()
{
	// -- 先にフォントハンドルを全て解放 -- //
	for (auto& pair : mHandles)
	{
		DeleteFontToHandle(pair.second);
	}
	mHandles.clear();

	// -- 次にフォントファイルを解放(ハンドルが使っている可能性があるため後) -- //
	for (auto& pair : mFontFiles)
	{
		RemoveFontFile(pair.second);
	}
	mFontFiles.clear();
}