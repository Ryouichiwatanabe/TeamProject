#include "GameScene.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "UIManager.h"

// -- GameObject -- //
#include "Stage.h"
#include "Player.h"
#include "FakePlayer.h" // デバック用
#include "StageBlock.h"
#include "MovingBlock.h"

#include <string>

// -- UI -- //
#include "PlayerShapeUI.h"

#include "Debug.h"
#include "GameConfig.h"
#include "Goal.h"

#include "FontManager.h"


#include "InputManager.h"

void GameScene::Initialize()
{
	mbPreviousMouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// 初期化処理
	mpPlayer =
		this
		->GetObjectManager()
		->CreateObject<Player>();

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
	mTypewriter = TypewriterText(3); //3フレームに1文字送る設定
	mMessageMotion = FloatingMotion(90, 4.0f);
	std::string startMessage = "Stage" + std::to_string(mnStageNumber) + "スタート! ゴールをめざせ!";
	mTypewriter.SetText(startMessage);
	mbIsMessageActive = true;//メッセージ表示開始

    auto* fakePlayer = 
        this
        ->GetObjectManager()
        ->CreateObject<FakePlayer>();

    auto* testUI =
        this
        ->GetUIManager()
        ->CreateUI<PlayerShapeUI>();

	mPauseBoardHandle = Master::mpResource->LoadGraphics("Resource/UI/PauseBoard.png");
}

void GameScene::Update(float deltaTime)
{
	if (mbIsClear)
	{
		UpdateClear();
		return;
	}

	UpdatePause();

	if (mbIsPaused)
	{
		deltaTime = 0.0f;
	}


	//====メッセージ表示中の制御====//
	if (mbIsMessageActive)
	{
		mTypewriter.Update();
		mMessageMotion.Update();
		// 左クリック または Zキー/SPACEキーが押されたか判定
		bool currentMouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
		bool isClickTriggered = currentMouseLeft && !mbPreviousMouseLeft;
		mbPreviousMouseLeft = currentMouseLeft;

		// キーボード入力判定（必要に応じて）
		static bool prevActionKey = false;
		bool curActionKey = (CheckHitKey(KEY_INPUT_Z) != 0 || CheckHitKey(KEY_INPUT_SPACE) != 0);
		bool isKeyTriggered = curActionKey && !prevActionKey;
		prevActionKey = curActionKey;

		if (isClickTriggered || isKeyTriggered)
		{
			if (!mTypewriter.IsFinished())
			{
				// ① まだ流れている最中なら全文スキップ
				mTypewriter.Skip();
			}
			else
			{
				// ② 既に全部出ているならメッセージを閉じてゲーム開始
				mbIsMessageActive = false;
			}
		}

		// メッセージ表示中はゲーム本編の進行（移動など）を止める場合
		return;
	}


	//==通常時：ステージ番号の文字を更新==//
	mStageTextMotion.Update();


	// 更新処理
	Scene::Update(deltaTime);

	if (mpGoal != nullptr)
	{
		bool shapeMatched = mpGoal->IsShapeMatched(mpPlayer);
		bool withinDistance = mpGoal->IsWithinDistance(mpPlayer);

		Debug::Print("Goal Shape : ", shapeMatched);
		Debug::Print("Goal Distance : ", withinDistance);

		if (shapeMatched && withinDistance)
		{
			mbIsClear = true;
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
	if (mbIsMessageActive)
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
	}


    if (mbIsPaused)
    {
        DrawPause();
    }

	if (mbIsClear)
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

	DrawString(
		570, 250,
		"CLEAR!",
		GetColor(255, 255, 255)
	);

	DrawString(
		500, 350,
		"ステージクリア！",
		GetColor(255, 255, 255)
	);

	// ステージ選択へ戻るボタン
	DrawBox(
		500, 400,
		780, 470,
		GetColor(80, 80, 80),
		TRUE
	);

	DrawBox(
		500, 400,
		780, 470,
		GetColor(255, 255, 255),
		FALSE
	);

	DrawString(
		570, 425,
		"ステージ選択へ戻る",
		GetColor(255, 255, 255)
	);
}

void GameScene::UpdateClear()
{
	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);

	bool currentMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	bool mouseLeftDown =
		currentMouseLeft && !mbPreviousMouseLeft;

	if (mouseLeftDown &&
		mouseX >= 500 && mouseX <= 780 &&
		mouseY >= 400 && mouseY <= 470)
	{
		Master::mpSceneManager->RequestScene(
			SCENE_TYPE::STAGE_SELECT_SCENE
		);
		
	}

	mbPreviousMouseLeft = currentMouseLeft;
}

void GameScene::UpdatePause()
{
	bool currentP = CheckHitKey(KEY_INPUT_P) != 0;

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