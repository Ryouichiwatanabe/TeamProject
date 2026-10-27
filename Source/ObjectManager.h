#pragma once

#include <vector>

/* memo : 
* // -- 概要 -- //
* Objectを総括して管理するクラス。
* mObjectsに全Objを収納しています。
* 
* 所有権はScene
* 
* 
* // -- 時間がない人へ重要事項 -- //
*	Objectを生成するときはCreateObject()を介してください。
*	ObjectManagerに登録されず、管理外のObjになってしまいます。
* 
* 
* // -- 時間がある人へ、ObjectManagerの説明 -- //
*	・従来のObjectManagerへの登録方法は、
*	Object2D / Object3Dのコンストラクタで
*		「Master::mpSceneManager->GetCurrentScene()->GetObjectManager()->AddObject(this);」
*	をしていたのでGameSceneなどでは、
*		「mpPlayer = new Player();」
*	のような書き方で良かったですが、
*	ObjectManager - Object2D の関係性を直感的に表したかったので、
*	今回はObjectManagerを介した生成を強制しています。
*	(もし使いづらいなどのお声があれば対応します)
*	
*	・このシステムの最たる利点は、
*	FindObject() / FindObjects()で指定した型のまま取り出せるということです。
*	templateという見慣れない書き方をしているのはそのためです。
*	例えばPlayerのデータがほしいとき、
*	
*	旧)
*	GameScene.cpp
*	auto* obj = Master::mpSceneManager-> ... GetObject(Tag);	// 型はObject2D
*	auto* player = dynamic_cast<Player*>(obj);					// dynamic_castでPlayer型へ
* 
*	新)
*	GameScecne.cpp
*	auto* player = this->GetSceneManager()->FindObject(class);	// Player型をそのまま取得
* 
*	で省略して書くことができます。
*	Object間でのデータのやり取りは頻繁に行われるので、
*	これはとても価値のあることです。
*/

class GameObject;

class ObjectManager
{
public:
	
	ObjectManager() = default;
	~ObjectManager() = default;

	void Update(float _deltaTime);
	void Draw();

	void ResolveCollision();

	template <class T, class...Args>
	T* CreateObject(Args&&... args) {
		static_assert(
			std::is_base_of_v<GameObject, T>,
			"T must derive from GameObject"
			);

		auto obj = new T(std::forward<Args>(args)...);
		 obj->Initialize(this);	// 初期化処理を生成と同時に行う

		mObjects.push_back(obj);

		return obj;
	}

	template<class T>
	T* FindObject() {
		for (auto& object : mObjects) {
			if (auto ptr = dynamic_cast<T*>(object)) {
				return ptr;
			}
		}

		return nullptr;
	}

	template<class T>
	std::vector<T*> FindObjects() {
		static_assert(
			std::is_base_of_v<GameObject, T>,
			"T must derive from GameObject"
			);

		std::vector<T*> result;

		for (auto& object : mObjects) {
			if (auto ptr = dynamic_cast<T*>(object)) {
				result.push_back(ptr);
			}
		}

		return result;
	}

	const std::vector<GameObject*> GetObjects() { return mObjects; }
	
	void Clear();	// 全てのObjectを解放する(シーン終了時など)

private:
	void RemoveDestroyObjects();

private:
	std::vector<GameObject*> mObjects;	// Objのリスト
};


