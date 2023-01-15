#pragma once

#include <glm/glm.hpp>

struct SpriteComponent
{
	int width;
	int height;

	SpriteComponent(int with = 10, int height = 10)
	{
		this->width = with;
		this->height = height;
	}
};