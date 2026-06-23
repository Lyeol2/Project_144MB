#include "Scene.h"
#include <algorithm>

Scene::Scene()
{
    m_tileMap = new TileMap();
}

Scene::~Scene()
{
	for (Entity* entity : m_entities)
	{
		delete entity;
	}
	m_entities.clear();
    delete m_tileMap;
}

void Scene::Initialize()
{
    if (m_tileMap) m_tileMap->Initialize();

	for (Entity* entity : m_entities)
	{
		entity->Initialize();
	}
}

void Scene::Update(float deltaTime)
{
	for (Entity* entity : m_entities)
	{
		if (entity->IsActive())
		{
			entity->Update(deltaTime);
		}
	}
}

void Scene::Render(Renderer* renderer)
{
    if (m_tileMap) m_tileMap->Render(renderer);

	for (Entity* entity : m_entities)
	{
		if (entity->IsActive())
		{
			entity->Render(renderer);
		}
	}
}

void Scene::AddEntity(Entity* entity)
{
	if (entity != nullptr)
	{
		m_entities.push_back(entity);
	}
}

void Scene::RemoveEntity(Entity* entity)
{
	auto it = std::find(m_entities.begin(), m_entities.end(), entity);
	if (it != m_entities.end())
	{
		m_entities.erase(it);
	}
}
