#pragma once
#include "SceneTransition.h"


class Scene;

enum class SCENE_TYPE
{
	SCENE_NONE = 0,		// 定義なし
	TITLE_SCENE = 1,    // タイトル
	STAGE_SELECT_SCENE = 2, // ステージセレクト
	GAME_SCENE = 3,		// GameScene
};

class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager() = default;
	
	void Initialize();					// 初期処理
	void Update(float _deltaTime);		// 更新処理
	void Draw();						// 描画処理
	void Finalize();					// 終了処理

	//void ChangeSceneIfNeeded();	// シーン遷移（切り替え処理）が必要な状態なら遷移処理する
	void SetNextScene(SCENE_TYPE next) { mnNextSceneType = next;}	// 次に遷移するシーンの設定

	Scene* GetCurrentScene() { return mpCurrentScene; }			// 現在シーンの取得
	SCENE_TYPE GetCurrentSceneType() { return mnSceneType; }	// シーンタイプの取得関数
	// 次のシーンを予約・フェードアウト開始
	void RequestScene(SCENE_TYPE _nextType);
	
	void RequestGameScene(int _stageNumber);	// ステージ番号をStageSelectのみで設定するための窓口 & 次シーンの予約
	int GetStageNumber() const;					// 最後に選択されたステージ番号の取得

	// -- Quitの要求に用いる -- //
	void RequestQuit() { mbQuitRequested = true; }
	[[nodiscard]] bool IsQuitRequested() const { return mbQuitRequested; }

private:
	void ChangeSceneIfNeeded();

private:
	SCENE_TYPE mnSceneType = SCENE_TYPE::SCENE_NONE;		// 現在シーンのタイプ
	SCENE_TYPE mnNextSceneType = SCENE_TYPE::SCENE_NONE;	// 次シーンのタイプ
	Scene* mpCurrentScene = nullptr;						// 現在シーンのポインタ

	SceneTransition mTransition{ 30 };	// 遷移演出

	bool mbQuitRequested = false;		// Quitの要求があった場合、それを示す

	int mnStageNumber = 1;	//選択されたStageの番号(1~8)
};