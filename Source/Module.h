#pragma once
#include <memory>

class GameObject;

// GameObjectに追加して機能を拡張するコンポーネントの基底クラス 
class Module
{
public:
	virtual ~Module() = default;

	virtual void Initialize() {};		// 初期化処理
	virtual void Finalize() {};			// 終了処理
	virtual void Update() {};		//更新処理
	virtual void Draw() {};		//描画処理

	// コンポーネントが有効であるかどうかのアクセサ
	bool IsEnabled() const { return mbEnabled; }
	void SetEnabled(bool _enabled) { mbEnabled = _enabled; }

	// 付与先のGameObjectを返す
	// memo : 主にColliderのような2つのObjから比較する際に使用
	GameObject* GetGameObject() { return mpGameObject; }

protected:
	GameObject* mpGameObject = nullptr;	// このコンポーネントを所持するGameObject

private:
	friend class GameObject;	// コンポーネント追加時に所有元GameObjectを設定するため
	bool mbEnabled = true;		// trueならUpdate・Drawなどの処理を行う
};