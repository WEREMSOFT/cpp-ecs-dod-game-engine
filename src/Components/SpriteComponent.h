#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct SpriteComponent
{
	int width;
	int height;
	int zIndex;

	std::string assetId;
	SDL_Rect srcRect;

	SpriteComponent(const std::string assetId = "", int with = 10, int height = 10, int zIndex = 0, int srcRectX = 0, int srcRectY = 0)
	{
		this->zIndex = zIndex;
		this->width = with;
		this->height = height;
		this->assetId = assetId;
		this->srcRect = {srcRectX, srcRectY, with, height};		
	}
};