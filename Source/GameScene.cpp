#include "GameScene.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "UIManager.h"
//-- Effect --//
#include "EffectClear.h"
#include "EffectSmokeSpriteSheet.h"

// -- GameObject -- //
#include "Stage.h"
#include "Player.h"
#include "FakePlayer.h" // デバック用
#include "StageBlock.h"
#include "MovingBlock.h"

#include <string>
#include <DxLib.h>

// -- UI -- //
#include "PlayerShapeUI.h"
#include "CollectibleIconUI.h"
#include "Collectible.h"
#include "ClearCollectibleUI.h"

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
	auto* pSmokeData = Master::mpResource->LoadDivGraphics("Resource/Smoke3.png", 16, 4, 4);

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
			0.05f,           // 1コマの表示秒数
			true,            // ★ bIsLoop: true でループ再生
			false,           // 加算ブレンド
			1.0,             // スケール
			0.0              // 角度
		);
	}
	// 初期化処理
	mpPlayer = this->GetObjectManager()->CreateObject<Player>();
	mpPlayer->InitPiece();

	auto* stage = new Stage();

	std::string stageFileName =
		"Resource/Stage/Stage"
		+ std::to_string(mnStageNumber)
		+ ".csv";

	stage->Load(stageFileName);

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
	mStageTextMotion = UILabel(10, 10, stageText, GetColor(0, 0, 0), StageNumberFontHandle,stageMotion);


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

	for (int i = 0; i < 3; i++)
	{
		mpClearCollectibleUI[i] =
			this
			->GetUIManager()
			->CreateUI<ClearCollectibleUI>();

		mpClearCollectibleUI[i]->SetGraph(
			stage->GetCollectibleFilename(i)
		);

		mpClearCollectibleUI[i]
			->GetTransform()
			.SetAnchor(
				VGet(0.0f, 0.0f, 0.0f)
			);

		mpClearCollectibleUI[i]
			->GetTransform()
			.SetPosition(
				VGet(
					550.0f + i * 90.0f,
					315.0f,
					0.0f
				)
			);

		mpClearCollectibleUI[i]
			->GetTransform()
			.SetScale(
				VGet(0.5f, 0.5f, 1.0f)
			);

		mpClearCollectibleUI[i]->SetEnabled(false);
	}

	mPauseBoardHandle = Master::mpResource->LoadGraphics("Resource/UI/PauseBoard.png");
}
void GameScene::Update(float deltaTime)
{
	// 1. エフェクトマネージャーの更新（クリア中も動かすため最優先）
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Update(deltaTime);
	}

	// 2. クリア状態の処理
	if (mbIsClear)
	{
		// エフェクトを1秒ほど見せてからクリアUIを操作可能にする
		mClearTimer += deltaTime;
		if (mClearTimer >= 1.0f)
		{
			UpdateClear();
		}
		return;
	}

	UpdatePause();

	if (mbIsPaused)
	{
		deltaTime = 0.0f;
	}

	//==通常時：ステージ番号の文字を更新==//
	mStageTextMotion.Update();

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

		// クリアエフェクトの再生
		if (mpEffectManager != nullptr)
		{
			// 画面の中心座標（1280x720 の中央）で花火エフェクトを再生
			constexpr float screenCenterX = ScreenConfig::SCREEN_WIDTH / 2.0f;   // 640.0f
			constexpr float screenCenterY = ScreenConfig::SCREEN_HEIGHT / 2.0f;  // 360.0f

			mpEffectManager->Play<EffectClear>(screenCenterX, screenCenterY);
		}
	}
}



void GameScene::Draw()
{
    DrawBox(
        0, 0,
        1280, 720,
        GetColor(217, 198, 143),
        TRUE
    );
	//Stage文字描画
	mStageTextMotion.Draw();


    Debug::Print(
        "InputMode : ",
        static_cast<int>(InputManager::GetInstance().GetInputMode())
    );

    Debug::Draw();
    Scene::Draw();


	// ★ メッセージウィンドウ描画
/*	if (mbIsMessageActive)
	{
		// 背景の半透明黒帯ウィンドウ
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		DrawBox(140, 520, 1140, 680, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// ウィンドウの外枠
		DrawBox(140, 520, 1140, 680, GetColor(255, 255, 255), FALSE);

		//TypeWriter用
		int offsetY = static_cast<int>(mMessageMotion.GetOffsetY());

		// 文字の描画（PixelMplus10 フォントを適用）
		mTypewriter.Draw(170, 550 + offsetY, GetColor(255, 255, 255), mMessageFontHandle);

		// 読み終えて次に進める合図（点滅アイコンなど）
		if (mTypewriter.IsFinished())
		{
			DrawString(1100, 640, "▼", GetColor(255, 255, 255));
		}
	}*/


	// エフェクト描画（オブジェクトの上、クリア暗転UIの下）
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Draw();
	}

	if (mbIsPaused)
	{
		DrawPause();
	}

	// エフェクトが弾けた後（1秒後）にクリア画面を表示
	if (mbIsClear && mClearTimer >= 1.0f)
	{
		DrawClear();
	}
}

void GameScene::DrawPause()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(
		0, 0,
		1280, 720,
		GetColor(0, 0, 0),
		TRUE
	);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// Pauseタイトル
	DrawStringToHandle(
		570,
		200,
		"Pause",
		GetColor(255, 255, 255),
		StageNumberFontHandle
	);

	const int buttonWidth = 280;
	const int buttonHeight = 70;

	// 再開ボタン
	if (mPauseBoardHandle != -1)
	{
		DrawExtendGraph(
			500,
			300,
			500 + buttonWidth,
			300 + buttonHeight,
			mPauseBoardHandle,
			TRUE
		);
	}

	DrawStringToHandle(
		605,
		325,
		"再開",
		GetColor(0, 0, 0),
		mMessageFontHandle
	);

	// ステージ選択
	if (mPauseBoardHandle != -1)
	{
		DrawExtendGraph(
			500,
			400,
			500 + buttonWidth,
			400 + buttonHeight,
			mPauseBoardHandle,
			TRUE
		);
	}

	DrawStringToHandle(
		505,
		425,
		"ステージ選択に戻る",
		GetColor(0, 0, 0),
		mMessageFontHandle
	);

	// タイトルへ戻る
	if (mPauseBoardHandle != -1)
	{
		DrawExtendGraph(
			500,
			500,
			500 + buttonWidth,
			500 + buttonHeight,
			mPauseBoardHandle,
			TRUE
		);
	}

	DrawStringToHandle(
		540,
		525,
		"タイトルへ戻る",
		GetColor(0, 0, 0),
		mMessageFontHandle
	);
}

void GameScene::DrawClear()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);

	DrawBox(
		0, 0,
		1280, 720,
		GetColor(0, 0, 0),
		TRUE
	);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// クリアパネル
	SetDrawBlendMode(
		DX_BLENDMODE_ALPHA,
		220
	);

	// 外側
	DrawBox(
		400,
		180,
		880,
		600,
		GetColor(80, 65, 45),
		TRUE
	);

	// 内側
	SetDrawBlendMode(
		DX_BLENDMODE_ALPHA,
		210
	);

	DrawBox(
		410,
		190,
		870,
		590,
		GetColor(245, 230, 190),
		TRUE
	);

	// 中央部分
	SetDrawBlendMode(
		DX_BLENDMODE_ALPHA,
		230
	);

	DrawBox(
		425,
		205,
		855,
		575,
		GetColor(45, 40, 35),
		TRUE
	);

	SetDrawBlendMode(
		DX_BLENDMODE_NOBLEND,
		0
	);

	// 上側のアクセントライン
	DrawBox(
		425,
		205,
		855,
		215,
		GetColor(210, 170, 90),
		TRUE
	);

	// 下側のアクセントライン
	DrawBox(
		425,
		565,
		855,
		575,
		GetColor(210, 170, 90),
		TRUE
	);

	int clearOffsetY =
		static_cast<int>(mClearTextMotion.GetOffsetY());

	DrawString(
		620,
		220 + clearOffsetY,
		"CLEAR!",
		GetColor(255, 255, 255)
	);

	// ステージ選択へ戻るボタン
	DrawBox(
		500, 450,
		780, 520,
		GetColor(80, 80, 80),
		TRUE
	);

	DrawBox(
		500, 450,
		780, 520,
		GetColor(255, 255, 255),
		FALSE
	);

	DrawString(
		570, 475,
		"ステージ選択へ戻る",
		GetColor(255, 255, 255)
	);

	for (int i = 0; i < 3; i++)
	{
		// アイテムごとに少し時間差をつける
		float startTime = i * 0.15f;

		if (mfClearItemTimer < startTime)
		{
			continue;
		}

		float animationTime =
			mfClearItemTimer - startTime;

		const float animationDuration = 0.35f;

		float rate =
			animationTime / animationDuration;

		if (rate > 1.0f)
		{
			rate = 1.0f;
		}

		// 小さい状態から元の大きさへ
		float scale =
			0.2f + 0.3f * rate;

		mpClearCollectibleUI[i]
			->GetTransform()
			.SetScale(
				VGet(
					scale,
					scale,
					1.0f
				)
			);

		mpClearCollectibleUI[i]->Draw();

		// 獲得状態
		if (mbIsCollectibleCollected[i])
		{
			DrawString(
				530 + i * 90,
				365,
				"獲得",
				GetColor(255, 255, 255)
			);
		}
		else
		{
			SetDrawBlendMode(
				DX_BLENDMODE_ALPHA,
				150
			);

			DrawString(
				530 + i * 90,
				365,
				"未獲得",
				GetColor(160, 160, 160)
			);

			SetDrawBlendMode(
				DX_BLENDMODE_NOBLEND,
				0
			);
		}
	}
}

void GameScene::UpdateClear()
{

	mfClearItemTimer += 1.0f / 60.0f;

	mClearTextMotion.Update();

	for (int i = 0; i < 3; i++)
	{
		if (mbIsCollectibleCollected[i])
		{
			mpClearCollectibleUI[i]->SetCollected(true);
		}
	}

	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);

	bool currentMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	bool mouseLeftDown =
		currentMouseLeft && !mbPreviousMouseLeft;

	if (mouseLeftDown &&
		mouseX >= 500 && mouseX <= 780 &&
		mouseY >= 430 && mouseY <= 500)
	{
		Master::mpSceneManager->RequestScene(
			SCENE_TYPE::STAGE_SELECT_SCENE
		);
	}

	mbPreviousMouseLeft = currentMouseLeft;
}

void GameScene::UpdatePause()
{
	bool currentP = InputManager::GetInstance().GetButtonDown(Button::Pause);

	if (currentP && !mbPreviousP)
	{
		mbIsPaused = !mbIsPaused;
	}

	mbPreviousP = currentP;

	if (!mbIsPaused)
	{
		return;
	}

	// マウス座標
	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);

	// 左クリック判定
	bool currentMouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
	bool mouseLeftDown = currentMouseLeft && !mbPreviousMouseLeft;

	//  再開ボタン
	if (mouseLeftDown &&
		mouseX >= 500 && mouseX <= 780 &&
		mouseY >= 300 && mouseY <= 370)
	{
		mbIsPaused = false;
	}

	//  ステージ選択へ戻るボタン
	if (mouseLeftDown &&
		mouseX >= 500 && mouseX <= 780 &&
		mouseY >= 400 && mouseY <= 470)
	{
		
		// フェードアウトを開始してステージセレクトへ
		Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
	}

	// タイトルへ戻るボタン
	if (mouseLeftDown &&
		mouseX >= 500 && mouseX <= 780 &&
		mouseY >= 500 && mouseY <= 570)
	{
		
		// フェードアウトを開始してタイトルへ
		Master::mpSceneManager->RequestScene(SCENE_TYPE::TITLE_SCENE);
	}

	mbPreviousMouseLeft = currentMouseLeft;
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