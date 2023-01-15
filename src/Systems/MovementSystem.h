#pragma once

#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System
{
public:

	MovementSystem() {
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

			Logger::Log("Entity " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y) + ")");
		}

	}
};