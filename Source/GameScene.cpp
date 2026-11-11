#include "GameScene.h"

// -- SceneBase -- //
#include "Master.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "UIManager.h"

// -- Manager(グローバル) -- //
#include "InputManager.h"
#include "SoundManager.h"
#include "ItemRecord.h"

// -- Config -- //
#include "GameConfig.h"
#include "ScreenConfig.h"

// -- GameObject -- //
#include "Stage.h"
#include "Player.h"
#include "Goal.h"
#include "Item.h"
#include "ItemTable.h"

// -- UI -- //
#include "StageBackGround.h"
#include "PlayerShapeUI.h"
#include "PauseUI.h"
#include "ClearUI.h"
#include "ItemHUD.h"
#include "RespawnUI.h"

// -- Effect -- //
#include "EffectManager.h"
#include "EffectClear.h"
#include "KitchenFrame.h"


// -- 外部ライブラリ -- //
#include <DxLib.h>

// -- 標準ライブラリ -- //
#include <string>

void GameScene::Initialize()
{
	// -- ステージの生成 -- //
	auto* stage = new Stage();
	std::string stageFileName =
		"Resource/CSV/Stage/Stage"
		+ std::to_string(Master::mpSceneManager->GetStageNumber())
		+ ".csv";
	stage->Load(stageFileName);

	// -- 「Player変形UI」の生成 -- //
	mpPlayer = this->GetObjectManager()->FindObject<Player>();
	if (mpPlayer)
		mpShapeUI = GetUIManager()->CreateUI<PlayerShapeUI>();

	// -- 「背景」の生成 -- //
	mpBackgroundUI = GetUIManager()->CreateUI<StageBackGround>();

	// -- 「Goal」の取得 -- //
	mpGoal = this->GetObjectManager()->FindObject<Goal>();

	// -- 「ClearUI」の生成 -- //
	mpClearUI = GetUIManager()->CreateUI<ClearUI>();
	
	// -- 「PauseUI」の生成 -- //
	mpPauseUI = GetUIManager()->CreateUI<PauseUI>();
	mpPauseUI->SetEnabled(false);

	// -- 「ItemHUD」の生成 -- //
	mpItemHUD = GetUIManager()->CreateUI<ItemHUD>(Master::mpSceneManager->GetStageNumber());
	mpPauseUI->AddMenuContent(mpItemHUD);

	// -- 「RespawnUI」の生成 -- //
	mpRespawnUI = GetUIManager()->CreateUI<RespawnUI>();
	// -- 「KitchenFrame」の生成 --//
	mpKitchenFrame = new KitchenFrame();

	SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Game, true);
}

void GameScene::Update(float deltaTime)
{
	switch (mState)
	{
	case PlayState::Playing:
		UpdatePlaying(deltaTime);
		break;

	case PlayState::Cleared:
		UpdateCleared(deltaTime);
		break;
	}
}

void GameScene::UpdatePlaying(float deltaTime)
{
	// -- Pauseの処理 -- //
	UpdatePauseInput();

	// Playerの更新より先に開閉する(開いたフレームからPlayerと移動床を止めるため)
	UpdateShapeInput();

	// ★修正：ポーズ中 または 変形画面(mpShapeUI)を開いている間は、ゲーム世界(移動床など)の時間を止める
	const bool isPaused = (mpPauseUI != nullptr && mpPauseUI->IsEnabled());
	const bool isShapeEditing = (mpShapeUI != nullptr && mpShapeUI->IsEnabled());
	SetWorldTimeScale((isPaused || isShapeEditing) ? 0.0f : 1.0f);

	UpdateRestartInput(deltaTime);

	// 更新処理
	Scene::Update(deltaTime);
	UpdateItems();

	if (mpGoal != nullptr && mpGoal->IsPlayerTouching())
		EnterClear();
}

void GameScene::EnterClear()
{
	// Item取得状態の記録
	ItemRecord::GetInstance().Commit(Master::mpSceneManager->GetStageNumber(), mPickedItems);

	// ゲーム世界を停止
	SetWorldTimeScale(0.0f);

	SoundManager::GetInstance()->StopBGM();
	SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_GOALVOICE);

	GetEffectManager()->Play<EffectClear>();
	mpPauseUI->SetEnabled(false);
	if (mpRespawnUI != nullptr)
		mpRespawnUI->SetEnabled(false);
	mpClearUI->Show(mPickedItems);

	mState = PlayState::Cleared;
}

void GameScene::UpdateCleared(float deltaTime)
{
	// ゴールボイスが終わったらBGMを流す
	if (!SoundManager::GetInstance()->IsPlayingSE(SoundManager::SOUND_SE::SE_GOALVOICE))
		SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title, false);

	// 時間倍率はEnterClearで0にしてあるので、世界は止まったまま
	Scene::Update(deltaTime);
}

void GameScene::Draw()
{
	// 背景（最背面なので一番最初に描く）
	if (mpBackgroundUI != nullptr)
	{
		mpBackgroundUI->DrawBackground();
	}

	Scene::Draw();

	if (mpKitchenFrame != nullptr)
	{
		mpKitchenFrame->Draw();
	}

}

void GameScene::UpdateShapeInput()
{
	if (mpShapeUI != nullptr && InputManager::GetInstance().GetButtonDown(Button::Shape))
		mpShapeUI->OnShapeInput();
}

void GameScene::UpdateItems()
{
	for (auto* item : GetObjectManager()->FindObjects<Item>())
	{
		if (!item->IsPicked())
			continue;

		const int slot = item->GetSlot();
		if (slot < 0 || slot >= GameConfig::MAX_ITEMS_PER_STAGE)
			continue;

		if (mPickedItems[slot])		// 取得済みなら何もしない
			continue;

		// ここに来るのは「取った最初のフレーム」だけ
		mPickedItems[slot] = true;
		mpItemHUD->SetPicked(slot);

		if (mpBackgroundUI != nullptr)
			mpBackgroundUI->SetPicked(slot);
	}
}

void GameScene::UpdatePauseInput()
{
	if (mpPauseUI != nullptr && InputManager::GetInstance().GetButtonDown(Button::Pause))
		mpPauseUI->OnPauseInput();
}

void GameScene::UpdateRestartInput(float deltaTime)
{
	if (mpRespawnUI == nullptr || mpPlayer == nullptr)
		return;

	// ポーズ中は表示しない
	mpRespawnUI->SetEnabled(!mpPauseUI->IsEnabled());		// ← 追加

	// ポーズ中・変形UIを開いている間は受け付けない
	const bool canRestart = !mpPauseUI->IsEnabled() && !mpPlayer->IsShapeEditing();
	const bool isHeld = canRestart && InputManager::GetInstance().GetButton(Button::Restart);

	if (mpRespawnUI->UpdateHold(isHeld, deltaTime))
		mpPlayer->Respawn();
}

void GameScene::Finalize()
{
	if (mpKitchenFrame != nullptr)
	{
		delete mpKitchenFrame;
		mpKitchenFrame = nullptr;
	}
	Scene::Finalize();
}