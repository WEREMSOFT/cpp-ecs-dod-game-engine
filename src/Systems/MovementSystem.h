#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System
{
public:

	MovementSystem() {
		name = MOVEMENT_SYSTEM;
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
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