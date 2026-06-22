#pragma once
#include <vector>
#include "Entity.h"

class Renderer;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Initialize();
	virtual void Update(float deltaTime);
	virtual void Render(Renderer* renderer);

	void AddEntity(Entity* entity);
	void RemoveEntity(Entity* entity);
	const std::vector<Entity*>& GetEntities() const { return m_entities; }

protected:
	std::vector<Entity*> m_entities;
};
