#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/KeyboardControllerComponent.h"
#include "../Components/CameraFollowComponent.h"

#include "../Events/KeyPressedEvent.h"
#include "../EventBus/EventBus.h"

class ProjectileEmitSystem: public System
{
	public:
		ProjectileEmitSystem()
		{
			RequireComponent<ProjectileEmitterComponent>();
			RequireComponent<TransformComponent>();
		}

		void OnKeyPressed(KeyPressedEvent& event)
		{
			for(auto entity: GetSystemEntities())
			{
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

				if(entity.HasTag(TagsIdEnum::PLAYER))
				{
					switch(event.code)
					{
						case SDLK_SPACE:
							const auto transform = entity.GetComponent<TransformComponent>();
							const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

							auto projectile = entity.registry->CreateEntity();
							projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDurationMs);
							glm::vec2 velocity = {0, 0};
							if(rigidBody.velocity.x > 0)
							{
								velocity.x = projectileEmitter.projectileVelocity.x;
							} else if(rigidBody.velocity.x < 0){
								velocity.x = -projectileEmitter.projectileVelocity.x;
							} else if(rigidBody.velocity.y < 0){
								velocity.y = -projectileEmitter.projectileVelocity.y;
							} else if(rigidBody.velocity.y > 0){
								velocity.y = projectileEmitter.projectileVelocity.y;
							}

							auto position = transform.position;

							const auto sprite = entity.GetComponent<SpriteComponent>();
							position.x += sprite.width / 2;
							position.y += sprite.height / 2;

							projectile.AddComponent<TransformComponent>(position, glm::vec2(1.), 0);
							projectile.AddComponent<RigidBodyComponent>(velocity);
							projectile.AddComponent<SpriteComponent>(TextureIdEnum::BULLET_IMAGE, 4, 4, 4);
							projectile.AddComponent<BoxColliderComponent>(4, 4);
							projectile.Group(GroupsIdEnum::PROJECTILES);

							projectileEmitter.lastEmissionTimeMs = SDL_GetTicks();

							break;
					}
				}
			}
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
		}

		void Update(std::unique_ptr<Registry>& registry)
		{
			for(auto entity: GetSystemEntities())
			{
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const auto transform = entity.GetComponent<TransformComponent>();

				if(projectileEmitter.repeatFrequencyMs != 0 && static_cast<int>(SDL_GetTicks()) - projectileEmitter.lastEmissionTimeMs > projectileEmitter.repeatFrequencyMs)
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
					projectile.AddComponent<SpriteComponent>(TextureIdEnum::BULLET_IMAGE, 4, 4, 4);
					projectile.AddComponent<BoxColliderComponent>(4, 4);
					projectile.Group(GroupsIdEnum::PROJECTILES);
					projectileEmitter.lastEmissionTimeMs = SDL_GetTicks();
				}

			}
		}
};