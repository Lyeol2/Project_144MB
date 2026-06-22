#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
	Player();
	virtual ~Player();

	virtual void Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(Renderer* renderer) override;
	
	float speed;
};
