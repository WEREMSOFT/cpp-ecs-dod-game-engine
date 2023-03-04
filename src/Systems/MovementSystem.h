#pragma once
#include <string>

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Events/CollisionEvent.h"

class MovementSystem : public System
{
public:

	MovementSystem() {
		name = MOVEMENT_SYSTEM;
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void onCollision(CollisionEvent &event)
	{
		Entity a = event.a;
		Entity b = event.b;

		if(a.BelongsToGroup(GroupsIdEnum::ENEMIES) && b.BelongsToGroup(GroupsIdEnum::OBSTACLES))
		{
			OnEnemyHitObstacle(a, b);
		}

		if(b.BelongsToGroup(GroupsIdEnum::ENEMIES) && a.BelongsToGroup(GroupsIdEnum::OBSTACLES))
		{
			OnEnemyHitObstacle(b, a);
		}

	}

	void OnEnemyHitObstacle(Entity enemy, Entity obstacle)
	{
		if(enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>())
		{
			auto &rigidBodyComponent = enemy.GetComponent<RigidBodyComponent>();
			auto &spriteComponent = enemy.GetComponent<SpriteComponent>();
			rigidBodyComponent.velocity.x *= -1;
			if(rigidBodyComponent.velocity.x > 0)
				spriteComponent.flip = SDL_FLIP_NONE;
			else
				spriteComponent.flip = SDL_FLIP_HORIZONTAL;
		}
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
	}

	void Update(double deltaTime)
	{

		for(auto entity: GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			bool isEntityOutsideMap = (
				transform.position.x < 0 ||
				transform.position.x > Game::mapWidth ||
				transform.position.y < 0 ||
				transform.position.y > Game::mapHeight
			);

			if(isEntityOutsideMap && !entity.HasTag(TagsIdEnum::PLAYER))
				entity.Kill();

			if(entity.HasTag(TagsIdEnum::PLAYER))
			{
				const auto boxCollider = entity.GetComponent<BoxColliderComponent>();
				transform.position.x = fminf(Game::mapWidth - boxCollider.with, fmaxf(0., transform.position.x));
				transform.position.y = fminf(Game::mapHeight - boxCollider.height, fmaxf(0., transform.position.y));
			}
		}

	}
};