#include "StageSelectScene.h"
#include "Master.h"
#include "SceneManager.h"
#include "DxLib.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "StageClearData.h"
#include "StageSelectData.h"
#include "CsvLoader.h"
#include <string>

void StageSelectScene::Initialize()
{
	mbPreviousMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	mnSelectedStage = 0;

	mBackgroundHandle =
		LoadGraph("Resource/StageSelect/StageSelectMaterial/notClear.png");

	for (int i = 0; i < 8; i++)
	{
		mStageClearHandles[i] =
			LoadGraph(
				StageSelectData::GetClearImageFilename(i + 1).c_str()
			);
	}
}

void StageSelectScene::Update(float _deltaTime)
{
	/*if (Master::mpSceneManager->IsChangingScene())
	{
		return;
	}*/

	int mouseX = 0;
	int mouseY = 0;

	GetMousePoint(
		&mouseX,
		&mouseY
	);

	bool currentMouseLeft =
		(GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

	bool mouseLeftDown =
		currentMouseLeft &&
		!mbPreviousMouseLeft;

	if (mouseLeftDown)
	{
		for (int i = 0; i < 8; i++)
		{
			if (!IsMouseInsideStageButton(
				i,
				mouseX,
				mouseY))
			{
				continue;
			}

			mnSelectedStage = i;

			Master::mpSceneManager->SetStageNumber(
				i + 1
			);

			Master::mpSceneManager->RequestScene(
				SCENE_TYPE::GAME_SCENE
			);

			break;
		}
	}

	mbPreviousMouseLeft =
		currentMouseLeft;

	Scene::Update(_deltaTime);
}

void StageSelectScene::Draw()
{
	DrawBackground();

	DrawStageClearImages();

	Scene::Draw();
}

void StageSelectScene::DrawBackground()
{
	if (mBackgroundHandle == -1)
		return;

	int screenWidth = 0;
	int screenHeight = 0;

	GetDrawScreenSize(
		&screenWidth,
		&screenHeight
	);

	int imageWidth = 0;
	int imageHeight = 0;

	GetGraphSize(
		mBackgroundHandle,
		&imageWidth,
		&imageHeight
	);

	if (imageWidth <= 0 || imageHeight <= 0)
		return;

	float scaleX =
		static_cast<float>(screenWidth) /
		static_cast<float>(imageWidth);

	float scaleY =
		static_cast<float>(screenHeight) /
		static_cast<float>(imageHeight);

	float scale =
		(scaleX > scaleY)
		? scaleX
		: scaleY;

	int drawWidth =
		static_cast<int>(imageWidth * scale);

	int drawHeight =
		static_cast<int>(imageHeight * scale);

	int drawX =
		(screenWidth - drawWidth) / 2;

	int drawY =
		(screenHeight - drawHeight) / 2;

	DrawExtendGraph(
		drawX,
		drawY,
		drawX + drawWidth,
		drawY + drawHeight,
		mBackgroundHandle,
		TRUE
	);
}

void StageSelectScene::DrawStageClearImages()
{
	if (mBackgroundHandle == -1)
		return;

	int screenWidth = 0;
	int screenHeight = 0;

	GetDrawScreenSize(
		&screenWidth,
		&screenHeight
	);

	int backgroundWidth = 0;
	int backgroundHeight = 0;

	GetGraphSize(
		mBackgroundHandle,
		&backgroundWidth,
		&backgroundHeight
	);

	if (backgroundWidth <= 0 || backgroundHeight <= 0)
		return;

	float scaleX =
		static_cast<float>(screenWidth) /
		static_cast<float>(backgroundWidth);

	float scaleY =
		static_cast<float>(screenHeight) /
		static_cast<float>(backgroundHeight);

	float scale =
		(scaleX > scaleY) ? scaleX : scaleY;

	int drawWidth =
		static_cast<int>(backgroundWidth * scale);

	int drawHeight =
		static_cast<int>(backgroundHeight * scale);

	int drawX =
		(screenWidth - drawWidth) / 2;

	int drawY =
		(screenHeight - drawHeight) / 2;

	for (int i = 0; i < 8; i++)
	{
		int stageNumber = i + 1;

		if (!StageClearData::IsStageCleared(stageNumber))
			continue;

		int clearGraphHandle =
			mStageClearHandles[i];

		if (clearGraphHandle == -1)
			continue;

		int imageWidth = 0;
		int imageHeight = 0;

		GetGraphSize(
			clearGraphHandle,
			&imageWidth,
			&imageHeight
		);

		if (imageWidth <= 0 || imageHeight <= 0)
		{
			continue;
		}

		const StageButton& button =
			mStageButtons[i];

		int buttonCenterX =
			static_cast<int>(
				button.x + button.width * 0.5f
				);

		int buttonCenterY =
			static_cast<int>(
				button.y + button.height * 0.5f
				);

		int screenCenterX =
			drawX +
			static_cast<int>(
				buttonCenterX * scale
				);

		int screenCenterY =
			drawY +
			static_cast<int>(
				buttonCenterY * scale
				);

		float imageScaleX =
			(button.width * scale) /
			static_cast<float>(imageWidth);

		float imageScaleY =
			(button.height * scale) /
			static_cast<float>(imageHeight);

		float imageScale =
			((imageScaleX < imageScaleY)
				? imageScaleX
				: imageScaleY) * 2.15f;

		int drawImageWidth =
			static_cast<int>(
				imageWidth * imageScale
				);

		int drawImageHeight =
			static_cast<int>(
				imageHeight * imageScale
				);

		DrawExtendGraph(
			screenCenterX - drawImageWidth / 2,
			screenCenterY - drawImageHeight / 2,
			screenCenterX + drawImageWidth / 2,
			screenCenterY + drawImageHeight / 2,
			clearGraphHandle,
			FALSE
		);

	}
}

bool StageSelectScene::IsMouseInsideStageButton(
	int _stageIndex,
	int _mouseX,
	int _mouseY)
{
	if (_stageIndex < 0 || _stageIndex >= 8)
	{
		return false;
	}

	if (mBackgroundHandle == -1)
	{
		return false;
	}

	int screenWidth = 0;
	int screenHeight = 0;

	GetDrawScreenSize(
		&screenWidth,
		&screenHeight
	);

	int imageWidth = 0;
	int imageHeight = 0;

	GetGraphSize(
		mBackgroundHandle,
		&imageWidth,
		&imageHeight
	);

	if (imageWidth <= 0 || imageHeight <= 0)
	{
		return false;
	}

	// 背景画像を画面いっぱいに表示するときの倍率
	float scaleX =
		static_cast<float>(screenWidth)
		/ static_cast<float>(imageWidth);

	float scaleY =
		static_cast<float>(screenHeight)
		/ static_cast<float>(imageHeight);

	float scale =
		(scaleX > scaleY)
		? scaleX
		: scaleY;

	int drawWidth =
		static_cast<int>(imageWidth * scale);

	int drawHeight =
		static_cast<int>(imageHeight * scale);

	int drawX =
		(screenWidth - drawWidth) / 2;

	int drawY =
		(screenHeight - drawHeight) / 2;

	// マウス座標を画像側の座標へ変換
	float imageX =
		(_mouseX - drawX) / scale;

	float imageY =
		(_mouseY - drawY) / scale;

	const StageButton& button =
		mStageButtons[_stageIndex];

	return
		imageX >= button.x &&
		imageX <= button.x + button.width &&
		imageY >= button.y &&
		imageY <= button.y + button.height;
}

void StageSelectScene::Finalize()
{
	// 終了処理

	// タイトルBGMを停止
	SoundManager::GetInstance()->StopBGM();
	if (mBackgroundHandle != -1)
	{
		DeleteGraph(mBackgroundHandle);
		mBackgroundHandle = -1;
	}

	for (int i = 0; i < 8; i++)
	{
		if (mStageClearHandles[i] != -1)
		{
			DeleteGraph(mStageClearHandles[i]);
			mStageClearHandles[i] = -1;
		}
	}
}