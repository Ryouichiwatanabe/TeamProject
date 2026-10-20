#pragma once
#include "SceneManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "EffectManager.h"

// #include "Loading.h"

/* memo : 
* Manager(ゲーム上に一つのみの管理クラス)を静的メンバ宣言するクラス
*/
class Master 
{
public:
	static SceneManager* mpSceneManager;
	static ResourceManager* mpResource;
	static FontManager* mpFontManager;
	static EffectManager* mpEffectManager;

	// static LoadingManager* mpLoading;
};
