#pragma once
#include "GameObject.h"

// memo : StageBlock
class Item : public GameObject {
public:
	~Item() = default;

	virtual void Init() override;			
	virtual void InitComponent() override;	
	virtual void Finalize() override;					
	virtual void Update(float _deltaTime) override;		
	virtual void Draw() override;	

private:

};