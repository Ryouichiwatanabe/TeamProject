#pragma once

// Todo : コンポジットする部品のインクルードをここへ
#include  "UITransform.h"
#include "UIGraphic.h"
#include "UIText.h"
#include "UIButton.h"

#include <vector>
#include <string>	// filenameの指定用

class UIManager;

class UI
{
public:
	UI() = default;
	virtual ~UI() = default;

	virtual void Init() {};		// 固有の初期化処理

	virtual void Update() {};		// 固有の更新処理
	virtual void Draw();

	void LoadUIGraph(std::string filename);

	// -- 親子の設定 -- //
	void AddChild(UI* _child) { mChildren.push_back(_child); }
	void SetParent(UI* _parent) { mpParent = _parent; }

	// -- レイヤーのアクセサ -- //
	void SetLayer(int _layer) { mnLayer = _layer; }
	int GetLayer() { return mnLayer; }

	bool IsEnabled();	// 有効であるかどうかを取得する
	void SetEnabled(bool _enabled) { mbIsEnabled = _enabled; }

	// -- Moduleの取得 -- //
	UITransform& GetTransform() { return mTransform; }
	UIGraphic& GetGraphic() { return mGraphic; }
	UIText& GetText() { return mText; }
	UIButton& GetButton() { return mButton; }

	// -- Managerの取得 -- //
	UIManager* GetUIManager() { return mpUIManager; }

private:
	friend class UIManager;
	UIManager* mpUIManager;
	void Initialize(UIManager* _manager);	// Manager内でUI生成時に通る

private:
	UI* mpParent;
	std::vector<UI*> mChildren;

	// Todo : コンポジットする部品の実体の宣言をここへ
	UITransform mTransform;
	UIGraphic mGraphic;
	UIText mText;
	UIButton mButton;

	bool mbIsEnabled = false;

	int mnLayer = 0;
};