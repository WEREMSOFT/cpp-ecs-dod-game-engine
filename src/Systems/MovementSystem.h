#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
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
		if(enemy.HasComponent<RigidBodyComponent>())
		{
			auto &rigidBodyComponent = enemy.GetComponent<RigidBodyComponent>();
			rigidBodyComponent.velocity.x *= -1;
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
		}

	}
};