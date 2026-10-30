#include "GameScene.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "UIManager.h"
#include "UIBackground.h"
#include "SoundManager.h"

//-- Effect --//
#include "EffectClear.h"
#include "EffectSmokeSpriteSheet.h"

// -- GameObject -- //
#include "Stage.h"
#include "Player.h"
#include "StageBlock.h"
#include "MovingBlock.h"

#include <string>
#include <DxLib.h>

// -- UI -- //
#include "PlayerShapeUI.h"
#include "CollectibleIconUI.h"
#include "Collectible.h"
#include "PauseUI.h"
#include "ClearUI.h"

#include "Debug.h"
#include "GameConfig.h"
#include "Goal.h"

#include "FontManager.h"

#include "EffectManager.h"


#include "InputManager.h"
#include "ScreenConfig.h"

void GameScene::Initialize()
{
	mbPreviousMouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// エフェクトマネージャーの生成（ヘッダーで std::unique_ptr<EffectManager> mpEffectManager を宣言）
	mpEffectManager = std::make_unique<EffectManager>();
	// 画像のロード
	auto* pSmokeData = Master::mpResource->LoadDivGraphics("Resource/Smok1e.png", 9, 3, 3);

	// 画面中央の座標を計算
	constexpr float screenCenterX = ScreenConfig::SCREEN_WIDTH / 2.0f;  // 640.0f
	constexpr float screenCenterY = ScreenConfig::SCREEN_HEIGHT / 2.0f; // 360.0f

	// 画面中央で常にループ再生
	if (pSmokeData != nullptr)
	{
		mpEffectManager->Play<EffectSmokeSpriteSheet>(
			screenCenterX,   // X座標（画面中央）
			screenCenterY,   // Y座標（画面中央）
			pSmokeData,      // 画像データ
			0.4f,           // 1コマの表示秒数
			true,            // ★ bIsLoop: true でループ再生
			false,           // 加算ブレンド
			2.0f,             // スケール
			0.0              // 角度
		);
	}
	// 初期化処理
	mpPlayer = this->GetObjectManager()->CreateObject<Player>();
	// -- 変形UIの生成 (Player生成後に行うこと : Init内でPlayerを探すため) -- //
	if (mpPlayer != nullptr)
	{
		mpShapeUI = GetUIManager()->CreateUI<PlayerShapeUI>();
	}

	auto* stage = new Stage();

	std::string stageFileName =
		"Resource/Stage/Stage"
		+ std::to_string(mnStageNumber)
		+ ".csv";

	stage->Load(stageFileName);

	// ==========================================
	// ★ 背景UIの生成とステージ反映
	// ==========================================
	mpBackgroundUI = GetUIManager()->CreateUI<UIBackground>();
	if (mpBackgroundUI != nullptr)
	{
		// 背景用として最背面のレイヤーに指定
		mpBackgroundUI->SetLayer(-100);

		// Stage クラスを渡して背景画像をロード＆適用
		mpBackgroundUI->SetStage(stage);

		// またはステージ番号を直接渡す場合:
		// mpBackgroundUI->SetStageByNumber(mnStageNumber);
	}

	mpGoal = this->GetObjectManager()->FindObject<Goal>();

	mpMovingBlock = this->GetObjectManager()->FindObject<MovingBlock>();


	// ★フォントの取得（サイズ32、太さ3、アンチエイリアス）
	// 引数を省略した場合は既定の太さ(-1)になります
	//mFontHandle = Master::mpFontManager->GetFont("メイリオ", 32, 3);

	mAddedFontHandle = static_cast<void*>(AddFontFile("Resource/Font/PixelMplus10-Regular.ttf"));

	// 2. アンチエイリアスを切ってフォントハンドルを作成
//    フォント名にはファイル名ではなく、フォント自体の「フォントファミリー名」を指定
	int fontType = DX_FONTTYPE_NORMAL; // アンチエイリアス無効（ドットがくっきり残る）

	StageNumberFontHandle = CreateFontToHandle("PixelMplus10", 80, -1, fontType);

	mMessageFontHandle = CreateFontToHandle("PixelMplus10", 30, -1, fontType);


	//Stage文字ラベルの初期化
	std::string stageText = "Stage" + std::to_string(mnStageNumber) + " ";
	FloatingMotion stageMotion(120, 15.0f);


	//Typewriter
	mMessageMotion = FloatingMotion(90, 4.0f);
	std::string startMessage = "Stage" + std::to_string(mnStageNumber) + "スタート! ゴールをめざせ!";
	mbIsMessageActive = true;//メッセージ表示開始

	mClearTextMotion = FloatingMotion(120, 5.0f);

   /* auto* fakePlayer = 
        this
        ->GetObjectManager()
        ->CreateObject<FakePlayer>();*/

	auto* testUI =
		this
		->GetUIManager()
		->CreateUI<PlayerShapeUI>();

	for (int i = 0; i < 3; i++)
	{
		mpCollectibleIconUI[i] =
			this
			->GetUIManager()
			->CreateUI<CollectibleIconUI>();

		mpCollectibleIconUI[i]->SetGraph(
			stage->GetCollectibleFilename(i)
		);

		mpCollectibleIconUI[i]
			->GetTransform()
			.SetAnchor(
				VGet(0.0f, 0.0f, 0.0f)
			);

		mpCollectibleIconUI[i]
			->GetTransform()
			.SetPosition(
				VGet(
					1080.0f + i * 80.0f,
					50.0f,
					0.0f
				)
			);

		mpCollectibleIconUI[i]
			->GetTransform()
			.SetScale(
				VGet(0.8f, 0.8f, 1.0f)
			);

		mpCollectibleIconUI[i]->SetEnabled(false);
	}

	// クリアUI
	mpClearUI =
		this->GetUIManager()
		->CreateUI<ClearUI>(
			stage->GetCollectibleFilename(0),
			stage->GetCollectibleFilename(1),
			stage->GetCollectibleFilename(2)
		);

	mpClearUI->SetEnabled(false);

	mpPauseUI =
		this->GetUIManager()
		->CreateUI<PauseUI>();
	mpPauseUI->SetEnabled(false);

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
		if (!SoundManager::GetInstance()->IsPlayingSE(SoundManager::SOUND_SE::SE_GOAL))
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

	auto player = GetObjectManager()->FindObject<Player>();

	// Collectibleの取得状態を確認
	auto collectibles =
		GetObjectManager()->FindObjects<Collectible>();

	for (auto collectible : collectibles)
	{
		int itemIndex = collectible->GetItemIndex();

		if (itemIndex < 0 || itemIndex >= 3)
		{
			continue;
		}

		if (collectible->IsPickedUp())
		{
			mbIsCollectibleCollected[itemIndex] = true;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (mbIsCollectibleCollected[i])
		{
			mpCollectibleIconUI[i]->SetCollected(true);
		}
	}

	if (mpGoal != nullptr &&
		mpGoal->IsPlayerTouching())
	{
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
			SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_GOAL);

			IsPlayingSE = true;

			mpEffectManager->Play<EffectClear>(screenCenterX, screenCenterY);
		}
		mpPauseUI->SetEnabled(false);
		mpClearUI->SetEnabled(true);
	}
}



void GameScene::Draw()
{
	// -- 描画時 -- //
	DrawStringToHandle(100, 100, "おちんこ祭り！！！",
		GetColor(255, 255, 255),              // 文字色
		Master::mpFontManager->GetHandle(FontId::Title),
		GetColor(0, 0, 0));                   // 縁の色

    Debug::Print(
        "InputMode : ",
        static_cast<int>(InputManager::GetInstance().GetInputMode())
    );

    Debug::Draw();
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

void GameScene::UpdateClear()
{
	for (int i = 0; i < 3; i++)
	{
		mpClearUI->SetCollected(
			i,
			mbIsCollectibleCollected[i]
		);
	}
}

void GameScene::UpdatePauseInput()
{
	if (InputManager::GetInstance().GetButtonDown(Button::Pause))
		mpPauseUI->ToggleEnabled();
}

void GameScene::SetStageNumber(int _stageNumber)
{
    mnStageNumber = _stageNumber;
}

void GameScene::Finalize()
{

	//===リソースの開放===//
	if (StageNumberFontHandle != -1)
	{
		DeleteFontToHandle(StageNumberFontHandle);
		StageNumberFontHandle = -1;
	}

	//===リソースの開放===//
	if (mMessageFontHandle != -1)
	{
		DeleteFontToHandle(mMessageFontHandle);
		mMessageFontHandle = -1;
	}

	if (mAddedFontHandle != nullptr)
	{
		//===登録したフォントの削除===//
		RemoveFontFile(static_cast<HANDLE>(mAddedFontHandle));
		mAddedFontHandle = nullptr;
	}



    // 終了処理
    Scene::Finalize();
}