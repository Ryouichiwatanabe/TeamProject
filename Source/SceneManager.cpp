#include "SceneManager.h"
#include "Scene.h"
#include "Master.h"

#include "GameScene.h"
#include "StageSelectScene.h"
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
	{
		auto* gameScene = new GameScene();

		gameScene->SetStageNumber(mnStageNumber);

		mpCurrentScene = gameScene;
		break;
	}
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

void SceneManager::SetStageNumber(int _stageNumber)
{
	mnStageNumber = _stageNumber;
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

