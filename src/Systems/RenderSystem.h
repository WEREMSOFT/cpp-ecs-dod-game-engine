#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL.h>

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<SpriteComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(SDL_Renderer* renderer) const
	{

		for (auto entity : GetSystemEntities())
		{
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto spriteComponent = entity.GetComponent<SpriteComponent>();
			
			SDL_Rect objRect = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				spriteComponent.width,
				spriteComponent.height
			};

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &objRect);

			Logger::Log("Entity " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y) + ")");
		}
	}
};