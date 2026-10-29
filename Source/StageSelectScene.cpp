#include "StageSelectScene.h"
#include "Master.h"
#include "SceneManager.h"
#include "DxLib.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "CsvLoader.h"
#include <string>

void StageSelectScene::Initialize()
{
	mbPreviousMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	// 最初はStage1を表示
	mnSelectedStage = 0;

	LoadStagePreview();
}

void StageSelectScene::Update(float _deltaTime)
{
	//Sceneが遷移中は入力を受け付けない
	if (Master::mpSceneManager->IsChangingScene())
	{
		return;
	}
	int mouseX;
	int mouseY;
	
	GetMousePoint(&mouseX, &mouseY);

	// ステージボタンの設定
	const int buttonWidth = 130;
	const int buttonHeight = 55;

	const int startX = 220;
	const int startY = 620;

	const int horizontalInterval = 140;

	// 左クリック
	bool currentMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	bool mouseLeftDown =
		currentMouseLeft && !mbPreviousMouseLeft;

	// ステージ切り替え矢印

	const int arrowSize = 50;

	const int leftArrowX = 130;
	const int rightArrowX = 1100;
	const int arrowY = 370;

	if (mouseLeftDown)
	{
		// 左矢印
		if (mouseX >= leftArrowX &&
			mouseX <= leftArrowX + arrowSize &&
			mouseY >= arrowY &&
			mouseY <= arrowY + arrowSize)
		{
			if (mnSelectedStage > 0)
			{
				mnSelectedStage--;

				LoadStagePreview();
			}
		}

		// 右矢印
		if (mouseX >= rightArrowX &&
			mouseX <= rightArrowX + arrowSize &&
			mouseY >= arrowY &&
			mouseY <= arrowY + arrowSize)
		{
			if (mnSelectedStage < 5)
			{
				mnSelectedStage++;

				LoadStagePreview();
			}
		}
	}

	// ステージボタン

	for (int i = 0; i < 6; i++)
	{
		int left =
			startX + i * horizontalInterval;

		int top = startY;

		int right =
			left + buttonWidth;

		int bottom =
			top + buttonHeight;

		// マウスを乗せたステージを選択
		if (mouseX >= left &&
			mouseX <= right &&
			mouseY >= top &&
			mouseY <= bottom)
		{
			if (mnSelectedStage != i)
			{
				mnSelectedStage = i;

				LoadStagePreview();
			}
		}

		// クリックしたらゲーム開始
		if (mouseLeftDown &&
			mouseX >= left &&
			mouseX <= right &&
			mouseY >= top &&
			mouseY <= bottom)
		{
			Master::mpSceneManager->SetStageNumber(
				i + 1
			);

			Master::mpSceneManager->RequestScene(
				SCENE_TYPE::GAME_SCENE
			);

			break;
		}
	}


	// 前フレームのマウス状態を保存
	mbPreviousMouseLeft = currentMouseLeft;

	Scene::Update(_deltaTime);
}

void StageSelectScene::Draw()
{
	// 背景
	DrawBox(
		0, 0,
		1280, 720,
		GetColor(217, 198, 143),
		TRUE
	);

	// 背景の上側に帯
	DrawBox(
		0, 0,
		1280, 150,
		GetColor(190, 170, 115),
		TRUE
	);

	// タイトル
	DrawString(
		520,
		45,
		"STAGE SELECT",
		GetColor(255, 255, 255)
	);

	// 説明
	DrawString(
		545,
		100,
		"ステージを選択してください",
		GetColor(0, 0, 0)
	);

	// ステージプレビュー

	char selectedStageName[32];

	sprintf_s(
		selectedStageName,
		"STAGE %d",
		mnSelectedStage + 1
	);

	DrawString(
		590,
		160,
		selectedStageName,
		GetColor(60, 50, 35)
	);

	DrawStagePreview();

	// 下部のステージ選択

	const int buttonWidth = 130;
	const int buttonHeight = 55;

	const int startX = 220;
	const int startY = 620;

	const int horizontalInterval = 140;

	for (int i = 0; i < 6; i++)
	{
		int left =
			startX + i * horizontalInterval;

		int top = startY;

		int right =
			left + buttonWidth;

		int bottom =
			top + buttonHeight;

		// 選択状態
		int color;

		if (mnSelectedStage == i)
		{
			color = GetColor(255, 220, 100);
		}
		else
		{
			color = GetColor(235, 225, 190);
		}

		// ボタン本体
		DrawBox(
			left,
			top,
			right,
			bottom,
			color,
			TRUE
		);

		// 外枠
		DrawBox(
			left,
			top,
			right,
			bottom,
			GetColor(60, 50, 35),
			FALSE
		);

		// ステージ番号
		char stageNumber[32];

		sprintf_s(
			stageNumber,
			"STAGE %d",
			i + 1
		);

		DrawString(
			left + 30,
			top + 20,
			stageNumber,
			GetColor(80, 65, 40)
		);
	}

	// 下部説明
	DrawString(
		500,
		690,
		"マウスでステージを選択",
		GetColor(80, 70, 50)
	);
	Scene::Draw();

}

void StageSelectScene::LoadStagePreview()
{
	std::string filename =
		"Resource/Stage/Stage"
		+ std::to_string(mnSelectedStage + 1)
		+ ".csv";

	mStageData = CsvLoader::Load(filename);
}

void StageSelectScene::DrawStagePreview()
{
	if (mStageData.empty())
		return;

	const int previewWidth = 900;
	const int previewHeight = 430;

	const int previewStartX = 190;
	const int previewStartY = 185;

	int rows = (int)mStageData.size();
	int cols = 0;

	for (const auto& row : mStageData)
	{
		if ((int)row.size() > cols)
		{
			cols = (int)row.size();
		}
	}

	if (cols <= 0 || rows <= 0)
		return;

	// 1マスの大きさをステージ全体が入るように計算
	float cellSizeX =
		(float)previewWidth / cols;

	float cellSizeY =
		(float)previewHeight / rows;

	float cellSize = cellSizeX;

	if (cellSizeY < cellSize)
	{
		cellSize = cellSizeY;
	}

	// ステージ全体を中央に配置
	float stageWidth = cols * cellSize;
	float stageHeight = rows * cellSize;

	float startX =
		previewStartX
		+ (previewWidth - stageWidth) / 2.0f;

	float startY =
		previewStartY
		+ (previewHeight - stageHeight) / 2.0f;

	// ステージ切り替え矢印

	const int arrowSize = 50;

	const int leftArrowX = 130;
	const int rightArrowX = 1100;
	const int arrowY = 370;

	// 左矢印
	DrawBox(
		leftArrowX,
		arrowY,
		leftArrowX + arrowSize,
		arrowY + arrowSize,
		GetColor(235, 225, 190),
		TRUE
	);

	DrawBox(
		leftArrowX,
		arrowY,
		leftArrowX + arrowSize,
		arrowY + arrowSize,
		GetColor(60, 50, 35),
		FALSE
	);

	DrawString(
		leftArrowX + 17,
		arrowY + 13,
		"<",
		GetColor(60, 50, 35)
	);

	// 右矢印
	DrawBox(
		rightArrowX,
		arrowY,
		rightArrowX + arrowSize,
		arrowY + arrowSize,
		GetColor(235, 225, 190),
		TRUE
	);

	DrawBox(
		rightArrowX,
		arrowY,
		rightArrowX + arrowSize,
		arrowY + arrowSize,
		GetColor(60, 50, 35),
		FALSE
	);

	DrawString(
		rightArrowX + 17,
		arrowY + 13,
		">",
		GetColor(60, 50, 35)
	);

	// ステージを描画
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < (int)mStageData[y].size(); x++)
		{
			int tile = mStageData[y][x];

			float left =
				startX + x * cellSize;

			float top =
				startY + y * cellSize;

			float right =
				left + cellSize;

			float bottom =
				top + cellSize;

			// ブロック
			if (tile == 1)
			{
				DrawBox(
					(int)left,
					(int)top,
					(int)right,
					(int)bottom,
					GetColor(80, 80, 80),
					TRUE
				);
			}
			// ゴール
			else if (tile == 2)
			{
				DrawBox(
					(int)left,
					(int)top,
					(int)right,
					(int)bottom,
					GetColor(255, 200, 50),
					TRUE
				);
			}

			// マスの枠
			DrawBox(
				(int)left,
				(int)top,
				(int)right,
				(int)bottom,
				GetColor(180, 180, 180),
				FALSE
			);
		}
	}
}

void StageSelectScene::Finalize()
{
	// 終了処理

	// タイトルBGMを停止
	SoundManager::GetInstance()->StopBGM();
}