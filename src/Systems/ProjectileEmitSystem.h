#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"

class ProjectileEmitSystem: public System
{
	public:
		ProjectileEmitSystem()
		{
			RequireComponent<ProjectileEmitterComponent>();
			RequireComponent<TransformComponent>();
		}

		void Update(std::unique_ptr<Registry>& registry)
		{
			for(auto entity: GetSystemEntities())
			{
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const auto transform = entity.GetComponent<TransformComponent>();

				if(static_cast<int>(SDL_GetTicks()) - projectileEmitter.lastEmissionTimeMs > projectileEmitter.repeatFrequencyMs)
				{
					auto position = transform.position;

					if(entity.HasComponent<SpriteComponent>())
					{
						const auto sprite = entity.GetComponent<SpriteComponent>();
						position.x += sprite.width / 2;
						position.y += sprite.height / 2;
					}
 
					auto projectile = registry->CreateEntity();
					projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDurationMs);
					projectile.AddComponent<TransformComponent>(position, glm::vec2(1.), 0);
					projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
					projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
					projectile.AddComponent<BoxColliderComponent>(4, 4);

					projectileEmitter.lastEmissionTimeMs = SDL_GetTicks();
				}

			}
		}
};