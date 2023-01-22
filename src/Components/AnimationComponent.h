#pragma once

#include <SDL2/SDL.h>

#include "../Game/ObjectNames.h"

struct AnimationComponent
{
	int numFrames;
	int currentFrame;
	int frameRateSpeed;
	bool isLoop;
	int startTime;
	ObjectNames name = ANIMATION_COMPONENT;


	AnimationComponent(int numFrames = 1, int frameRateSpeed = 4, bool isLoop = true)
	{
		this->numFrames = numFrames;
		this->currentFrame = 1;
		this->frameRateSpeed = frameRateSpeed;
		this->isLoop = isLoop;
		this->startTime = SDL_GetTicks();
	}
};