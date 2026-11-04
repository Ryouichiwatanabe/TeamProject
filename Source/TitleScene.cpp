#include "TitleScene.h"
// #include <DxLib.h>

#include "UI.h"
#include "UIManager.h"

#include "Master.h"
#include "SceneManager.h"

#include "InputManager.h"
//#include "FontManager.h"	// FontId
#include "SoundManager.h"

#include "ScreenConfig.h"	// 背景画像の拡縮

void TitleScene::Initialize()
{
	auto uiManager = GetUIManager();

	// -- 背景画像 -- //
	mpBackGround = uiManager->CreateUI<UI>();
	mpBackGround->LoadUIGraph("Resource/UI/Scene_Title/background.png");
	mpBackGround->SetEnabled(true);
	mpBackGround->SetLayer(0);
		// -- 拡縮
	const VECTOR size = mpBackGround->GetTransform().GetSize();
	const VECTOR scale = VGet(ScreenConfig::SCREEN_WIDTH / size.x, ScreenConfig::SCREEN_HEIGHT / size.y, 0.0f);
	mpBackGround->GetTransform().SetScale(scale);

	// -- ロゴ -- //
	//mpLogo = uiManager->CreateUI<UI>();
	//mpLogo->LoadUIGraph("Resource/UI/Scene_Title/TitleLogo.png");
	//mpLogo->SetEnabled(true);
	//mpLogo->SetLayer(1);
	//mpLogo->GetFloatingMotion().SetActive(true);	// 浮遊モーションを適用
	//mpLogo->GetTransform().SetPosition(VGet(0.0f, -200.0f, 0.0f));

	// -- 「Start」ボタン -- //
	mpStartButton = uiManager->CreateUI<UI>();
	mpStartButton->LoadUIGraph("Resource/UI/Scene_Title/Spoon.png");
	mpStartButton->SetEnabled(true);
	mpStartButton->SetLayer(1);
	mpStartButton->GetHoverScaler().SetActive(true);	// ボタンのホバー演出を適用
	mpStartButton->GetText().SetFontId(FontId::SubTitle);
	mpStartButton->GetText().SetColor(GetColor(255, 255, 255));
	mpStartButton->GetText().SetText("Start!");
	mpStartButton->GetTransform().SetPosition(VGet(50.0f, 250.0f, 0.0f));

	// -- 「Quit」ボタン -- //
	mpQuitButton = uiManager->CreateUI<UI>();
	mpQuitButton->LoadUIGraph("Resource/UI/Scene_Title/Fork.png");
	mpQuitButton->SetEnabled(true);
	mpQuitButton->SetLayer(1);
	mpQuitButton->GetHoverScaler().SetActive(true);	// ボタンのホバー演出を適用
	mpQuitButton->GetText().SetFontId(FontId::SubTitle);
	mpQuitButton->GetText().SetColor(GetColor(255, 255, 255));
	mpQuitButton->GetText().SetText("Quit");
	mpQuitButton->GetTransform().SetPosition(VGet(50.0f, 350.0f, 0.0f));

	// -- BGMの再生 -- //
	SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title, true);
}

void TitleScene::Update(float _deltaTime)
{
	// -- 「Start」ボタンを押した場合 -- //
	if (mpStartButton != nullptr
		&& mpStartButton->GetButton().IsClicked()) 
	{
		//  Confirm を消費し、共通のクリックSE判定をスキップ
		InputManager::GetInstance().ConsumeButton(Button::Confirm);
		SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);

		Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
	}

	// -- 「Quit」ボタンを押した場合 -- //
	if (mpQuitButton != nullptr
		&& mpQuitButton->GetButton().IsClicked())
	{
		//  Confirm を消費し、共通のクリックSE判定をスキップ
		InputManager::GetInstance().ConsumeButton(Button::Confirm);
		SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);

		Master::mpSceneManager->RequestQuit();
	}

	Scene::Update(_deltaTime);
}

void TitleScene::Draw()
{
	Scene::Draw();
}

void TitleScene::Finalize()
{
	Scene::Finalize();
}