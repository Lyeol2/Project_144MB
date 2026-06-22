#pragma once
#include "Texture.h"

class Sprite
{
public:
	Texture* texture = nullptr;
	int srcX = 0;
	int srcY = 0;
	int width = 0;
	int height = 0;
};
