#pragma once
#include <vector>
#include "Entity.h"
#include "TileMap.h"

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

    TileMap* GetTileMap() { return m_tileMap; }

protected:
	std::vector<Entity*> m_entities;
    TileMap* m_tileMap;
};
