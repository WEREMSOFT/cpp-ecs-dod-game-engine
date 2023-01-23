#pragma once
#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class DebugRenderSystem: public System
{
public:
	DebugRenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(SDL_Renderer* renderer)
	{
		auto entities = GetSystemEntities();

		for(auto entity: entities)
		{
			auto cComponent = entity.GetComponent<BoxColliderComponent>();
			auto tComponent = entity.GetComponent<TransformComponent>();

			if(cComponent.isColliding)
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			else
				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

			SDL_Rect rect = { 
								static_cast<int>(tComponent.position.x + cComponent.offset.x), 
								static_cast<int>(tComponent.position.y + cComponent.offset.y),
								cComponent.with,
								cComponent.height 
							};

			SDL_RenderDrawRect(renderer, &rect);
		}
	}
};