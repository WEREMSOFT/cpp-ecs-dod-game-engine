#pragma once
#include "../ECS/ECS.h"

#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"

#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"

#include "../Logger/Logger.h"

class DamageSystem: public System
{
	public:
		DamageSystem()
		{
			RequireComponent<BoxColliderComponent>();
			RequireComponent<HealthComponent>();
		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
		{
			eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
		}

		void OnProjectileHitsPlayer(Entity projectile, Entity player)
		{
			auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
			
			if(!projectileComponent.isFriendly)
			{
				auto& healthComponent = player.GetComponent<HealthComponent>();
				healthComponent.healthPercentage -= projectileComponent.hitPercentDamage;
				projectile.Kill();
				if(healthComponent.healthPercentage <= 0)
				{
					player.Kill();
				}
			}
		}

		void onCollision(CollisionEvent &event)
		{
			Entity a = event.a;
			Entity b = event.b;

			if(a.BelongsToGroup("projectiles") && b.HasTag("player"))
			{
				OnProjectileHitsPlayer(a, b);				
			}

			if(b.BelongsToGroup("projectiles") && a.HasTag("player"))
			{
				OnProjectileHitsPlayer(b, a);				
			}

			if(a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies"))
			{
				
			}

			if(b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies"))
			{
				
			}
		}

		void Update()
		{
		}
};