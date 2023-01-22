#pragma once
#include <glm/glm.hpp>
#include "../Game/ObjectNames.h"

struct TransformComponent
{
	glm::vec2 position;
	glm::vec2 scale;
	double rotation;

	ObjectNames name = TRANSFORM_COMPONENT;
	
	TransformComponent(glm::vec2 position = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1., 1.), double rotation = 0.)
	{
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
	}
};