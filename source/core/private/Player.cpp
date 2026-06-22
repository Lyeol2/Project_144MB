#include "Player.h"
#include "Input.h"
#include <windows.h> // For VK_ constants

Player::Player()
{
	speed = 200.0f; // 200 pixels per second
}

Player::~Player()
{
}

void Player::Initialize()
{
	Entity::Initialize();
}

void Player::Update(float deltaTime)
{
	Entity::Update(deltaTime);

	if (Input::GetKey(VK_LEFT))
	{
		x -= speed * deltaTime;
	}
	if (Input::GetKey(VK_RIGHT))
	{
		x += speed * deltaTime;
	}
	if (Input::GetKey(VK_UP))
	{
		y -= speed * deltaTime;
	}
	if (Input::GetKey(VK_DOWN))
	{
		y += speed * deltaTime;
	}
}

void Player::Render(Renderer* renderer)
{
	Entity::Render(renderer);
}
