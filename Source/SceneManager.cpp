#include "SceneManager.h"
#include "Scene.h"
#include "Master.h"

#include "GameScene.h"
#include "StageSelectScene.h"
#include "StageSelectScene_ex.h"
#include "TitleScene.h"
#include "TutorialScene.h"

void SceneManager::Initialize()
{
	// 最初のシーンを作る（フェードなしで即座に作成）
	mnNextSceneType = SCENE_TYPE::TUTORIAL_SCENE;
	ChangeSceneIfNeeded();

	// 真っ黒な状態から明転させる
	mTransition.StartFromBlack();
}


void SceneManager::RequestScene(SCENE_TYPE _nextType)
{
	if (mTransition.IsActive()) return;
	mnNextSceneType = _nextType;
	mTransition.Start();
}

void SceneManager::RequestGameScene(int _stageNumber)
{
	if (mTransition.IsActive()) return;   // 遷移中なら番号も書き換えない
	mnStageNumber = _stageNumber;
	RequestScene(SCENE_TYPE::GAME_SCENE);
}

void SceneManager::Update(float _deltaTime)
{
	if (mTransition.Update()) {
		ChangeSceneIfNeeded();        // 差し替えのタイミングだけ教えてもらう
	}
	if (!mTransition.IsActive() && mpCurrentScene) {
		mpCurrentScene->Update(_deltaTime);
	}
}

void SceneManager::Draw()
{
	if (mpCurrentScene)
		mpCurrentScene->Draw();

	mTransition.Draw();	// 必要に応じてフェードを演出
}

void SceneManager::ChangeSceneIfNeeded()
{
	if (mnSceneType == mnNextSceneType)
		return;

	//旧Sceneを解放
	if (mpCurrentScene != nullptr)
	{
		mpCurrentScene->Finalize();	// 旧シーンの終了処理をする
		delete mpCurrentScene;
		mpCurrentScene = nullptr;
	}

	mnSceneType = mnNextSceneType;	// 次シーンにするためシーンタイプを更新

	switch (mnSceneType)
	{
	case SCENE_TYPE::TITLE_SCENE:
		mpCurrentScene = new TitleScene();
		break;

	case SCENE_TYPE::STAGE_SELECT_SCENE:
		mpCurrentScene = new StageSelectScene();
		break;

	case SCENE_TYPE::GAME_SCENE:
		mpCurrentScene = new GameScene();
		break;

		case SCENE_TYPE::STAGE_SELECT_SCENE_EX:
		mpCurrentScene = new StageSelectScene_ex();
		break;

	case SCENE_TYPE::TUTORIAL_SCENE:
		mpCurrentScene = new TutorialScene();
		break;

	default:
		break;
	}
	
	if (mpCurrentScene != nullptr)
	{
		mpCurrentScene->Initialize();	// 新シーンの初期処理をする
	}
	
}

int SceneManager::GetStageNumber() const
{
	return mnStageNumber;
}

void SceneManager::Finalize()
{
	//メモリ解放
	if (mpCurrentScene != nullptr)
	{
		mpCurrentScene->Finalize();
		delete mpCurrentScene;
		mpCurrentScene = nullptr;

	}

}

