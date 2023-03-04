#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "../Game/ObjectNames.h"

struct SpriteComponent
{
	int width;
	int height;
	int zIndex;
	bool isFixed;

	TextureIdEnum assetId;
	SDL_Rect srcRect;
	SDL_RendererFlip flip;

	ObjectNames name = SPRITE_COMPONENT;

	SpriteComponent(const TextureIdEnum assetId = TextureIdEnum::NONE, int with = 10, int height = 10, int zIndex = 0, int srcRectX = 0, int srcRectY = 0, bool isFixed = false)
	{
		this->zIndex = zIndex;
		this->width = with;
		this->height = height;
		this->assetId = assetId;
		this->srcRect = {srcRectX, srcRectY, with, height};
		this->isFixed = isFixed;
		this->flip = SDL_FLIP_NONE;
	}
};