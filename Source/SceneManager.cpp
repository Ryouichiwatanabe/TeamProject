#include "SceneManager.h"
#include "Scene.h"
#include "Master.h"

#include "GameScene.h"
#include "StageSelectScene.h"
#include "TitleScene.h"

#include "FadeEffect.h"



SceneManager::SceneManager()
	:mnSceneType(SCENE_TYPE::SCENE_NONE)
	, mnNextSceneType(SCENE_TYPE::SCENE_NONE)
	, mpCurrentScene(nullptr)
	,mFadeEffect(30)//フェード時間(30フレーム = 0.5秒
	,mbIsChangingScene(false)
{

}

void SceneManager::Initialize()
{
	// -- 初期シーンの設定 -- //
	mnNextSceneType = SCENE_TYPE::TITLE_SCENE;
	ChangeSceneIfNeeded();

	mbIsChangingScene = true;
	//Game起動時にFadeInのEffect再生
	mFadeEffect.StartFadeIn();
}


void SceneManager::RequestScene(SCENE_TYPE _nextType)
{
	// 既に遷移中、または現在のシーンと同じなら処理しない（連打ガード）
	if (mbIsChangingScene || mnSceneType == _nextType)
	{
		return;
	}
		

	mnNextSceneType = _nextType;
	mbIsChangingScene = true;

	// 暗転（フェードアウト）開始
	mFadeEffect.StartFadeOut();
}

void SceneManager::Update(float _deltaTime)
{
	FadeEffect::State prevState = mFadeEffect.GetState();


	mFadeEffect.Update();

	//  シーン遷移シーケンスの制御
	if (mbIsChangingScene)
	{
		// フェードが終了したかチェック
		if (mFadeEffect.IsFinished())
		{
			if (prevState == FadeEffect::State::FadeOut)
			{
				//ここで最初のScene切り替え
				ChangeSceneIfNeeded();

				//FadeIn開始
				mFadeEffect.StartFadeIn();

			}
			//Sceneが完全に描画されてる
			else if (prevState == FadeEffect::State::FadeIn)
			{
				//遷移完了
				mbIsChangingScene = false;
			}
			
		}
	}
	//CurrentSceneの更新
			//遷移中はFadeOutやFadeInの処理は停止
	if (!mbIsChangingScene && mpCurrentScene != nullptr)
	{
		mpCurrentScene->Update(_deltaTime);
	}
	
}

void SceneManager::Draw()
{
	//CurrentSceneを描画
	if (mpCurrentScene != nullptr)
	{
		
		mpCurrentScene->Draw();
	}
	//最前面に黒幕を描画
	mFadeEffect.Draw(1280, 720);

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

