#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>

#include "../Game/ObjectNames.h"

class RenderSystem : public System
{
public:

	RenderSystem()
	{
		name = RENDER_SYSTEM;
		RequireComponent<SpriteComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera) const
	{
		struct RenderableEntity {
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};

		std::vector<RenderableEntity> renderableEntities;

		for(auto entity: GetSystemEntities())
		{
			RenderableEntity re = {
				entity.GetComponent<TransformComponent>(),
				entity.GetComponent<SpriteComponent>()
			};

			renderableEntities.emplace_back(re);
		}

		std::sort(renderableEntities.begin(), renderableEntities.end(), 
		[](const RenderableEntity& a, const RenderableEntity& b) 
		{   
			return (a.spriteComponent.zIndex < b.spriteComponent.zIndex);
		});

		for (auto renderableEntity : renderableEntities)
		{
			const auto transformComponent = renderableEntity.transformComponent;
			const auto spriteComponent = renderableEntity.spriteComponent;

			SDL_Texture* texture = assetStore->GetTexture(spriteComponent.assetId);

			SDL_Rect srcRect = spriteComponent.srcRect;

			SDL_Rect destRect = {
				static_cast<int>(transformComponent.position.x - camera.x),
				static_cast<int>(transformComponent.position.y - camera.y),
				static_cast<int>(spriteComponent.width * transformComponent.scale.x),
				static_cast<int>(spriteComponent.height * transformComponent.scale.y)};

			SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, transformComponent.rotation, NULL, SDL_FLIP_NONE);
		}
	}
};