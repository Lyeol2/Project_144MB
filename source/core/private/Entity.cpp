#include "Entity.h"
#include "Renderer.h"

Entity::Entity()
	: x(0.0f), y(0.0f), sprite(nullptr), collider(nullptr), m_isActive(true)
{
}

Entity::~Entity()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
	if (collider != nullptr)
	{
		delete collider;
		collider = nullptr;
	}
}

void Entity::Initialize()
{
}

void Entity::Update(float deltaTime)
{
}

void Entity::Render(Renderer* renderer)
{
	if (!m_isActive) return;

	if (sprite != nullptr && sprite->texture != nullptr)
	{
		renderer->DrawSprite((int)x, (int)y, sprite);
	}
}
