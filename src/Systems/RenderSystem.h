#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<SpriteComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore>& assetStore) const
	{

		for (auto entity : GetSystemEntities())
		{
			const auto transformComponent = entity.GetComponent<TransformComponent>();
			const auto spriteComponent = entity.GetComponent<SpriteComponent>();

			SDL_Texture* texture = assetStore->GetTexture(spriteComponent.assetId);


			SDL_Rect srcRect = spriteComponent.srcRect;

			SDL_Rect destRect = {
				static_cast<int>(transformComponent.position.x),
				static_cast<int>(transformComponent.position.y),
				static_cast<int>(spriteComponent.width * transformComponent.scale.x),
				static_cast<int>(spriteComponent.height * transformComponent.scale.y)};

			SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, transformComponent.rotation, NULL, SDL_FLIP_NONE);
		}
	}
};