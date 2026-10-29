#pragma once
#include "GameObject.h"

class StageBlock : public GameObject
{
public:
	~StageBlock() = default;

	virtual void Init() override;
	virtual void InitComponent() override;

	virtual void Update(float _deltaTime) override;
	virtual void Draw() override;

protected:
	virtual const char* GetModelFilename() const override
	{
		return "Resource/GameObject/StageBlock/StageBlock.png";
	}
};