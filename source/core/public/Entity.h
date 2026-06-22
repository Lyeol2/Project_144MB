#pragma once
#include "Sprite.h"
#include "Collider.h"

class Renderer;

class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void Initialize();
	virtual void Update(float deltaTime);
	virtual void Render(Renderer* renderer);

	// Transform
	float x;
	float y;

	// Components
	Sprite* sprite;
	Collider* collider;

	bool IsActive() const { return m_isActive; }
	void SetActive(bool active) { m_isActive = active; }

protected:
	bool m_isActive;
};
