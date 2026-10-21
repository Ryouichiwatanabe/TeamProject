#pragma once

class ObjectManager;
class UIManager;
class CollisionManager;
class EffectManager;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Initialize() = 0;					// 初期処理
	virtual void Finalize();						// 終了処理
	virtual void Update(float _deltaTime);			// 更新処理
	virtual void Draw();							// 描画処理



	ObjectManager* GetObjectManager() {return mpObjectManager;}
	UIManager* GetUIManager() { return mpUIManager; }
	CollisionManager* GetCollisionManager() {return mpCollisionManager;}
	EffectManager* GetEffectManager() { return mpEffectManager; }


private:
	ObjectManager* mpObjectManager;			// ObjectManager(所有権はScene)
	UIManager* mpUIManager;
	CollisionManager* mpCollisionManager;	// CollisionManager(所有権はScene)
	EffectManager* mpEffectManager;			//EffectManager(所有権はScene)
};
