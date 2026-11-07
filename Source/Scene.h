#pragma once
#include <algorithm>	// ats::max

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

	// ゲーム世界の時間倍率（0で停止、1で通常）。UIとEffectには影響しない
	void  SetWorldTimeScale(float _scale) { mfWorldTimeScale = (std::max)(0.0f, _scale); }
	float GetWorldTimeScale() const { return mfWorldTimeScale; }

	ObjectManager* GetObjectManager() { return mpObjectManager; }
	UIManager* GetUIManager() { return mpUIManager; }
	CollisionManager* GetCollisionManager() { return mpCollisionManager; }
	EffectManager* GetEffectManager() { return mpEffectManager; }

private:
	void UpdateWorld(float _worldDelta);    // ゲーム世界の更新処理(時間倍率の影響を受ける)
	void UpdateOverlay(float _realDelta);   // 演出の更新処理(常に実時間)

	float mfWorldTimeScale = 1.0f;

private:
	ObjectManager* mpObjectManager;			// ObjectManager(所有権はScene)
	UIManager* mpUIManager;
	CollisionManager* mpCollisionManager;	// CollisionManager(所有権はScene)
	EffectManager* mpEffectManager;			//EffectManager(所有権はScene)
};
