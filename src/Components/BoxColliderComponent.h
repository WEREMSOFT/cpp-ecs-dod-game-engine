#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent
{
	int with;
	int height;
	glm::vec2 offset;

	BoxColliderComponent(int with = 0, int height = 0, glm::vec2 offset = glm::vec2(0))
	{
		this->with = with;
		this->height = height;
		this->offset = offset;
	}
};