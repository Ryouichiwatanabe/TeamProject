#include"Scene.h"
#include "ObjectManager.h"
#include "UIManager.h"
#include "CollisionManager.h"
#include "EffectManager.h"
#include "InputManager.h"

Scene::Scene()
{
	mpObjectManager = new ObjectManager();
	mpUIManager = new UIManager();
	mpCollisionManager = new CollisionManager();
	mpEffectManager = new EffectManager();
}

Scene::~Scene()
{
	// -- 所有するポインタの解放 -- //
	delete mpObjectManager;
	mpObjectManager = nullptr;

	delete mpUIManager;
	mpUIManager = nullptr;

	delete mpCollisionManager;
	mpCollisionManager = nullptr;

	delete mpEffectManager;
	mpEffectManager = nullptr;
}

void Scene::Finalize() {
	// -- CollisionManagerに登録されているCollider群の解放 -- //
	if (mpCollisionManager != nullptr)
	{
		mpCollisionManager->Finalize();
	}

	// -- ObjectManagerの所有するObject群の解放 -- //
	if (mpObjectManager != nullptr)
	{
		mpObjectManager->Clear();
	}

	if (mpUIManager != nullptr)
	{
		mpUIManager->Clear();
	}

	//EffectManagerの所有するObject群の開放 --//
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Clear();
	}
}

void Scene::Update(float _deltaTime)
{
	UpdateWorld(_deltaTime * mfWorldTimeScale);
	UpdateOverlay(_deltaTime);
}

void Scene::UpdateWorld(float _worldDelta)
{
	if (mpObjectManager != nullptr)
		mpObjectManager->Update(_worldDelta);

	if (mpCollisionManager != nullptr)
		mpCollisionManager->Update();

	if (mpObjectManager != nullptr)
		mpObjectManager->ResolveCollision();
}

void Scene::UpdateOverlay(float _realDelta)
{
	if (mpUIManager != nullptr)
		mpUIManager->Update();

	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Update(_realDelta);
		mpEffectManager->PlayClickRipple();
	}
}
void Scene::Draw()
{
	// ★1. 背面エフェクト（背景の直後・ステージやキャラの後ろに湯気を描画）
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->DrawBackground();
	}

	// 2. ゲームオブジェクト（ステージブロック・プレイヤーなど）
	if (mpObjectManager != nullptr)
	{
		mpObjectManager->Draw();
	}

	// 3. UI（ポーズ画面・クリア画面など）
	if (mpUIManager != nullptr)
	{
		mpUIManager->Draw();
	}

	// 4. 前面エフェクト（ClickEffectなどを最前面に描画）
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Draw();
	}
}