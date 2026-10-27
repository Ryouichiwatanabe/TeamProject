#pragma once

#include <string>
#include <vector>
#include "DxLib.h"

class SoundManager
{
public:     // enum, struct などの定義

	// BGM の種類
	enum SOUND_BGM
	{

		BGM_Title = 0,          // タイトルBGM	

	};

	// SE の種類
	enum SOUND_SE
	{
		SE_DECIDE = 0,          // 決定音
		SE_MOVE = 1,
		SE_SELECTUI = 2,
	};

	static SoundManager* GetInstance();

private:

	static SoundManager* sInstance;

public:
	// コンストラクタ
	SoundManager();
	// デストラクタ
	~SoundManager();

	// 初期化
	void Initialize();
	// 終了処理
	void Finalize();

public:     // サウンド再生系

	// BGM再生
	// SOUND_BGM ... 鳴らしたいBGMの種類
	// isTop ... 最初から再生するかどうか
	// bool isTop = true -> デフォルト引数と呼ばれる書き方。関数を呼び出す際に渡されなくても認識される
	void PlayBGM(SOUND_BGM bgm, bool isTop = true);

	// SE再生
	// se ... 鳴らしたいSEの種類
	void PlaySE(SOUND_SE se);

	// BGMの読み込み
	void LoadBGM(SOUND_BGM bgm, std::string filename);

	// SEの読み込み
	void LoadSE(SOUND_SE se, std::string silename);

	// BGMの停止
	void StopBGM();

private:     // メンバ変数
	SOUND_BGM mnNowPlayingBgm;   // 現在再生されているBGMの種類
	SOUND_SE mnNowPlayingSe;     // 現在再生されているSEの種類

	std::vector<std::pair<SOUND_BGM, int>> mnBgmHandleList;   // 読みこんだBGMハンドルのリスト
	std::vector<std::pair<SOUND_SE, int>> mnSeHandleList;     // 読み込んだSEハンドルのリスト

	int mGameBGMHandle;

	bool mIsPaused = false;
};