#include "LoadScene.h"
#include "LoadUI.h"

#include "Master.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "InputManager.h"
#include "SoundManager.h"

#include <DxLib.h>
#include <algorithm>

void LoadScene::Initialize()
{
	mfTimer = 0.0f;
	mbReady = false;

	// -- 「LoadUI」の生成（ステージ番号に応じたTipsもここで設定される） -- //
	mpLoadUI = GetUIManager()->CreateUI<LoadUI>();
}

void LoadScene::Update(float _deltaTime)
{
	mfTimer += _deltaTime;

	// 100%までゲージが溜まったら準備完了
	const float progress = (std::min)(1.0f, mfTimer / LOAD_DURATION);
	mbReady = (progress >= 1.0f);

	mpLoadUI->SetLoadState(mfTimer, progress, mbReady);

	Scene::Update(_deltaTime);

	if (!mbReady)
		return;

	// 準備完了後、マウスクリック（左クリック）または 決定キー(Enter/Space)でゲームへ
	const bool isClicked = ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) ||
		InputManager::GetInstance().GetButtonDown(Button::Confirm);

	if (isClicked)
	{
		// 決定音を鳴らしてゲーム画面へ遷移
		SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);
		Master::mpSceneManager->RequestScene(SCENE_TYPE::GAME_SCENE);
	}
}

void LoadScene::Finalize()
{
	if (mpLoadUI != nullptr)
	{
		mpLoadUI->Release();
		mpLoadUI = nullptr;
	}

	Scene::Finalize();
}
