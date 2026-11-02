#include "TitleScene.h"
// #include <DxLib.h>

#include "UI.h"
#include "UIManager.h"

#include "Master.h"
#include "SceneManager.h"

#include "InputManager.h"
//#include "FontManager.h"	// FontId
#include "SoundManager.h"

void TitleScene::Initialize()
{
	auto uiManager = GetUIManager();

	// -- 背景画像 -- //
	mpBackGround = uiManager->CreateUI<UI>();
	mpBackGround->LoadUIGraph("Resource/UI/Scene_Title/test_background.png");
	mpBackGround->SetEnabled(true);
	mpBackGround->SetLayer(0);

	// -- ロゴ -- //
	mpLogo = uiManager->CreateUI<UI>();
	mpLogo->LoadUIGraph("Resource/UI/Scene_Title/TitleLogo.png");
	mpLogo->SetEnabled(true);
	mpLogo->SetLayer(1);
	mpLogo->GetFloatingMotion().SetActive(true);	// 浮遊モーションを適用
	mpLogo->GetTransform().SetPosition(VGet(0.0f, -200.0f, 0.0f));

	// -- 「Start」ボタン -- //
	mpStartButton = uiManager->CreateUI<UI>();
	mpStartButton->LoadUIGraph("Resource/UI/Scene_Title/StartButton.png");
	mpStartButton->SetEnabled(true);
	mpStartButton->SetLayer(1);
	mpStartButton->GetHoverScaler().SetActive(true);	// ボタンのホバー演出を適用
	mpStartButton->GetText().SetFontId(FontId::SubTitle);
	mpStartButton->GetText().SetColor(GetColor(100, 100, 100));
	mpStartButton->GetText().SetText("Start!");
	mpStartButton->GetTransform().SetPosition(VGet(0.0f, 100.0f, 0.0f));

	// -- BGMの再生 -- //
	SoundManager::GetInstance()->PlayBGM(SoundManager::SOUND_BGM::BGM_Title, true);
}

void TitleScene::Update(float _deltaTime)
{
	// -- 「スタート」ボタンを押した場合 -- //
	if (mpStartButton != nullptr
		&& mpStartButton->GetButton().IsClicked()) 
	{
		//  Confirm を消費し、共通のクリックSE判定をスキップ
		InputManager::GetInstance().ConsumeButton(Button::Confirm);
		SoundManager::GetInstance()->PlaySE(SoundManager::SOUND_SE::SE_SELECTUI);

		Master::mpSceneManager->RequestScene(SCENE_TYPE::STAGE_SELECT_SCENE);
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