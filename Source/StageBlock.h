#pragma once
#include "GameObject.h"

class StageBlock : public GameObject
{
public:
	~StageBlock() = default;

	void Init() override;
	void InitComponent() override;

	void Update(float _deltaTime) override;
	void Draw() override;

protected:
	const char* GetModelFilename() const override
	{
		return "Resource/Stage.png";
	}
};