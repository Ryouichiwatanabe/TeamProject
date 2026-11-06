#include "GameScene.h"

// -- SceneBase -- //
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
#include "UIBackground.h"
#include "PlayerShapeUI.h"
#include "PauseUI.h"
#include "ClearUI.h"
#include "ItemHUD.h"

// -- Effect -- //
#include "EffectManager.h"
#include "EffectClear.h"

// -- 外部ライブラリ -- //
#include <DxLib.h>

// -- 標準ライブラリ -- //
#include <string>

void GameScene::Initialize()
{
	// エフェクトマネージャーの生成（ヘッダーで std::unique_ptr<EffectManager> mpEffectManager を宣言）
	mpEffectManager = std::make_unique<EffectManager>();
	
	// -- ステージの生成 -- //
	auto* stage = new Stage();
	std::string stageFileName =
		"Resource/Stage/Stage"
		+ std::to_string(mnStageNumber)
		+ ".csv";
	stage->Load(stageFileName);
	
	// -- 「Player変形UI」の生成 -- //
	if (this->GetObjectManager()->FindObject<Player>())
		mpShapeUI = GetUIManager()->CreateUI<PlayerShapeUI>();
	
	// -- 「背景」の生成 -- //
	mpBackgroundUI = GetUIManager()->CreateUI<UIBackground>();
	if (mpBackgroundUI != nullptr) {
		mpBackgroundUI->SetStageByNumber(mnStageNumber);
	}

	mpGoal = this->GetObjectManager()->FindObject<Goal>();

	mpClearUI = GetUIManager()->CreateUI<ClearUI>(
		ItemTable::GetFilePath(mnStageNumber, 0),
		ItemTable::GetFilePath(mnStageNumber, 1),
		ItemTable::GetFilePath(mnStageNumber, 2)
	);

	mpPauseUI =
		this->GetUIManager()
		->CreateUI<PauseUI>();
	mpPauseUI->SetEnabled(false);
	mpItemHUD = GetUIManager()->CreateUI<ItemHUD>(mnStageNumber);
	mpPauseUI->AddMenuContent(mpItemHUD);

	SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Game, true);

}
void GameScene::Update(float deltaTime)
{
	// 1. エフェクトマネージャーの更新（クリア中も動かすため最優先）
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Update(deltaTime);
	}

	// SEチェック
	if (IsPlayingSE)
	{
		// SE_GOAL がまだ再生中かどうかをチェック
		if (!SoundManager::GetInstance()->IsPlayingSE(SoundManager::SOUND_SE::SE_GOALVOICE))
		{
			// 再生が終わったら、別のBGMを再生する
			SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title); // 例としてゲームBGMなど

			// 2回目以降に発火しないようフラグを下ろす
			IsPlayingSE = false;
		}
	}

	// 2. クリア状態の処理
	if (mbIsClear)
	{
		UpdateClear();

		GetUIManager()->Update();

		return;
	}

	// -- Pauseの処理 -- //
	UpdatePauseInput();
	if (mpPauseUI->IsEnabled())
		deltaTime = 0.0f;
	

	// Playerの更新より先に開閉する(開いたフレームからPlayerを止めるため)
	UpdateShapeUIToggle();

	// 更新処理
	Scene::Update(deltaTime);
	UpdateItems();

	if (mpGoal != nullptr && mpGoal->IsPlayerTouching())
	{
		ItemRecord::GetInstance().Commit(mnStageNumber, mPickedItems);	// Item取得状態の記録

		mbIsClear = true;
		mClearTimer = 0.0f; // タイマー初期化

		// タイトルBGMを停止
		SoundManager::GetInstance()->StopBGM();

		// クリアエフェクトの再生
		if (mpEffectManager != nullptr)
		{
			// 画面の中心座標（1280x720 の中央）で花火エフェクトを再生
			constexpr float screenCenterX = ScreenConfig::SCREEN_WIDTH / 2.0f;   // 640.0f
			constexpr float screenCenterY = ScreenConfig::SCREEN_HEIGHT / 2.0f;  // 360.0f

			// SE
			SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_GOALVOICE);

			IsPlayingSE = true;

			mpEffectManager->Play<EffectClear>(screenCenterX, screenCenterY);
		}
		mpPauseUI->SetEnabled(false);
		mpClearUI->SetEnabled(true);
	}
}



void GameScene::Draw()
{
	// 背景（最背面なので一番最初に描く）
	if (mpBackgroundUI != nullptr)
	{
		mpBackgroundUI->DrawBackground();
	}

    Scene::Draw();

	// エフェクト描画（オブジェクトの上、クリア暗転UIの下）
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Draw();
	}
}

void GameScene::UpdateShapeUIToggle()
{
	if (mpShapeUI == nullptr)
		return;

	if (!InputManager::GetInstance().GetButtonDown(Button::Shape))
		return;

	// ※企画書の「接地中のみ変形可」の条件は今回は加味しない
	if (mpShapeUI->IsEnabled())
		mpShapeUI->Close();
	else
		mpShapeUI->Open();
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
	}
}

void GameScene::UpdateClear()
{
	for (int i = 0; i < GameConfig::MAX_ITEMS_PER_STAGE; i++)
		mpClearUI->SetCollected(i, mPickedItems[i]);
}

void GameScene::UpdatePauseInput()
{
	if (InputManager::GetInstance().GetButtonDown(Button::Pause))
		mpPauseUI->OnPauseInput();
}

void GameScene::SetStageNumber(int _stageNumber)
{
    mnStageNumber = _stageNumber;
}

void GameScene::Finalize()
{
    Scene::Finalize();
}