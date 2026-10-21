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
	if (mpObjectManager != nullptr)
	{
		mpObjectManager->Update(_deltaTime);
	}

	if (mpCollisionManager != nullptr)
	{
		mpCollisionManager->Update();
	}

	if (mpObjectManager != nullptr)
	{
		mpObjectManager->ResolveCollision();
	}

	if (mpUIManager != nullptr)
	{
		mpUIManager->Update();
	}
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Update(_deltaTime);
	}
	UpdateClickEffect();

}

void Scene::Draw()
{
	if (mpObjectManager != nullptr)
	{
		mpObjectManager->Draw();
	}

	if (mpUIManager != nullptr)
	{
		mpUIManager->Draw();
	}
	if (mpEffectManager != nullptr)
	{
		mpEffectManager->Draw();
	}
}

// クリック状態を取得してエフェクトを生成する処理
// どのシーンでも使用するからここで呼び出し
void Scene::UpdateClickEffect()
{
	if (InputManager::GetInstance().GetButtonDown(Button::Confirm))
	{
		int mouseX = 0, mouseY = 0;
		GetMousePoint(&mouseX, &mouseY); // 現在のマウス座標を取得

		if (mpEffectManager != nullptr)
		{
			mpEffectManager->SpawnClickEffect(static_cast<float>(mouseX), static_cast<float>(mouseY));
		}
	}
}





