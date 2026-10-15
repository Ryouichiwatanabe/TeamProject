#include "SoundManager.h"
#include "DxLib.h"




SoundManager* SoundManager::sInstance = nullptr;

SoundManager* SoundManager::GetInstance()
{
	if (sInstance == nullptr)
	{
		sInstance = new SoundManager();
		sInstance->Initialize();
	}
	return sInstance;
}


SoundManager::SoundManager()
	: mnNowPlayingBgm((SOUND_BGM)-1)   // 初期状態は何も再生されない状態
	, mnNowPlayingSe((SOUND_SE)-1)     // 初期状態は何も再生されない状態
	, mIsPaused(false)
{

}

SoundManager::~SoundManager()
{

}

void SoundManager::Initialize()
{
	// BGM の読み込み
	//LoadBGM(SOUND_BGM::
	

	// SE の読み込み
//	LoadSE(SOUND_SE::SE_DECIDE, "Resource/SE/Decision.mp3");

	
	//3DBGM
	LoadBGM(SOUND_BGM::BGM_Title, "?????");
	
}

void SoundManager::Finalize()
{
	// BGMの破棄
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		DeleteSoundMem(it->second);
	}

	// SEの破棄
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		DeleteSoundMem(it->second);
	}

	if (sInstance)
	{
		delete sInstance;
		sInstance = nullptr;
	}
}

void SoundManager::PlayBGM(SOUND_BGM bgm, bool isTop)
{
	// 現在再生されているBGMと同じ、かつ、最初からの再生でないのであれば return する
	if (mnNowPlayingBgm == bgm && !isTop)
	{
		return;
	}

	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		// 一致した種類のBGMがあれば
		if (it->first == bgm)
		{
			// BGMをループ再生
			PlaySoundMem(it->second, DX_PLAYTYPE_LOOP, isTop);
			// 現在の再生種類を更新
			mnNowPlayingBgm = bgm;

			break;
		}
	}
}

void SoundManager::PlaySE(SOUND_SE se)
{
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		// 一致した種類のBGMがあれば
		if (it->first == se)
		{
			// SEをループ再生
			PlaySoundMem(it->second, DX_PLAYTYPE_BACK);
			// SEを一回だけ再生
			//PlaySoundMem(it->second, DX_PLAYTYPE_NORMAL, TRUE);
			// 現在の再生種類を更新
			mnNowPlayingSe = se;

			break;
		}
	}
}

void SoundManager::LoadBGM(SOUND_BGM bgm, std::string filename)
{
	bool check = false;   // 重複して読み込んでいるかどうか
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		if (it->first == bgm)
		{
			check = true;
			break;
		}
	}

	// 重複して読み込まれていたら何もしない
	if (check)
	{
		return;
	}

	// ファイル読み込み
	int handle = LoadSoundMem(filename.c_str());
	if (handle == -1)
	{
		return;   // 読み込み失敗していたら何もしない
	}

	// 読み込んだハンドルをリストに追加
	mnBgmHandleList.push_back(std::pair<SOUND_BGM, int>(bgm, handle));
}

void SoundManager::LoadSE(SOUND_SE se, std::string filename)
{
	bool check = false;   // 重複して読み込んでいるかどうか
	for (auto it = mnSeHandleList.begin(); it != mnSeHandleList.end(); it++)
	{
		if (it->first == se)
		{
			check = true;
			break;
		}
	}

	// 重複して読み込まれていたら何もしない
	if (check)
	{
		return;
	}

	// ファイル読み込み
	int handle = LoadSoundMem(filename.c_str());
	if (handle == -1)
	{
		return;   // 読み込み失敗していたら何もしない
	}

	// 読み込んだハンドルをリストに追加
	mnSeHandleList.push_back(std::pair<SOUND_SE, int>(se, handle));
}

void SoundManager::StopBGM()
{
	for (auto it = mnBgmHandleList.begin(); it != mnBgmHandleList.end(); it++)
	{
		if (it->first == mnNowPlayingBgm)
		{
			// BGMが再生されているかどうか
			if (CheckSoundMem(it->second))
			{
				StopSoundMem(it->second);   // BGM停止
				break;
			}
		}
	}
}

