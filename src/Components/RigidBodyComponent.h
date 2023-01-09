#pragma once
#include <glm/glm.hpp>

struct RigidBodyComponent
{
	glm::vec2 velocity;

	RigidBodyComponent(glm::vec2 velocity = {0., 0.})	
	{
		this->velocity = velocity;
	}
};