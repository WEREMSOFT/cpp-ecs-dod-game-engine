#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

class AnimationSystem: public System
{
public:

	AnimationSystem() {
		name = ANIMATION_SYSTEM;
		RequireComponent<SpriteComponent>();
		RequireComponent<AnimationComponent>();
	}

	void Update()
	{
		for(auto entity: GetSystemEntities())
		{
			auto& animation = entity.GetComponent<AnimationComponent>();
			auto& sprite = entity.GetComponent<SpriteComponent>();

			animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameRateSpeed / 1000) % animation.numFrames;

			sprite.srcRect.x = animation.currentFrame * sprite.width;
		}

	}
};