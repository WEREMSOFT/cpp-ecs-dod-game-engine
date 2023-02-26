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

		void OnProjectileHitsEnemy(Entity projectile, Entity enemy)
		{
			auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
			
			if(projectileComponent.isFriendly)
			{
				auto& healthComponent = enemy.GetComponent<HealthComponent>();
				healthComponent.healthPercentage -= projectileComponent.hitPercentDamage;
				projectile.Kill();
				if(healthComponent.healthPercentage <= 0)
				{
					enemy.Kill();
				}
			}
		}

		void onCollision(CollisionEvent &event)
		{
			Entity a = event.a;
			Entity b = event.b;

			if(a.BelongsToGroup(GroupsIdEnum::PROJECTILES) && b.HasTag(TagsIdEnum::PLAYER))
			{
				OnProjectileHitsPlayer(a, b);				
			}

			if(b.BelongsToGroup(GroupsIdEnum::PROJECTILES) && a.HasTag(TagsIdEnum::PLAYER))
			{
				OnProjectileHitsPlayer(b, a);				
			}

			if(a.BelongsToGroup(GroupsIdEnum::PROJECTILES) && b.BelongsToGroup(GroupsIdEnum::ENEMIES))
			{
				OnProjectileHitsEnemy(a, b);	
			}

			if(b.BelongsToGroup(GroupsIdEnum::PROJECTILES) && a.BelongsToGroup(GroupsIdEnum::ENEMIES))
			{
				OnProjectileHitsEnemy(b, a);
			}
		}

		void Update()
		{
		}
};