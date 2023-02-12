#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "../ECS/ECS.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

class RenderHealthBarSystem: public System
{
	public:
		RenderHealthBarSystem()
		{
			RequireComponent<HealthComponent>();
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect camera)
		{
			SDL_Rect rect = {0, 0, 20, 3};
			SDL_Color red = {255, 0, 0};
			SDL_Color yellow = {255, 255, 0};
			SDL_Color green = {0, 255, 0};

			for(auto entity: GetSystemEntities())
			{
				const auto health = entity.GetComponent<HealthComponent>();
				const auto transform = entity.GetComponent<TransformComponent>();
				const auto sprite = entity.GetComponent<SpriteComponent>();

				// Render health bar

				auto renderColor = green;

				if(health.healthPercentage <= 70) renderColor = yellow;
				if(health.healthPercentage <= 30) renderColor = red;

				SDL_SetRenderDrawColor(renderer, renderColor.r, renderColor.g, renderColor.b, 255);

				rect.x = transform.position.x;
				rect.y = transform.position.y + 10 + sprite.height;
				rect.w = health.healthPercentage / 5;

				SDL_RenderFillRect(renderer, &rect);

				// Render health pecentage (text)

				auto healthPercentageString = std::to_string(health.healthPercentage) + "%";

				SDL_Surface* surface = TTF_RenderText_Blended(
					assetStore->GetFont("charriot-font-small"), 
					healthPercentageString.c_str(),
					renderColor);

				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

				SDL_FreeSurface(surface);

				int labelWidth = 0;
				int labelHeight = 0;

				SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

				SDL_Rect dstRect = 
				{
					static_cast<int>(transform.position.x - camera.x),
					static_cast<int>(transform.position.y + sprite.height - camera.y),
					labelWidth,
					labelHeight
				};

				SDL_RenderCopy(renderer, texture, NULL, &dstRect);
			}
		}
};