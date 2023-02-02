#pragma once

#include <glm/glm.hpp>

struct ProjectileEmitterComponent
{

	glm::vec2 projectileVelocity;
	int repeatFrequencyMs;
	int projectileDurationMs;
	int hitPercentDamage;
	bool isFriendly;
	int lastEmissionTimeMs;

	ProjectileEmitterComponent(
		glm::vec2 projectileVelocity = glm::vec2(0), 
		int repeatFrequencyMs = 0, 
		int projectileDurationMs = 10000, 
		int hitPercentDamage = 10, 
		bool isFriendly = false
		)
	{

		this->projectileVelocity = projectileVelocity;
		this->repeatFrequencyMs = repeatFrequencyMs;
		this->projectileDurationMs = projectileDurationMs;
		this->hitPercentDamage = hitPercentDamage;
		this->isFriendly = isFriendly;
		this->lastEmissionTimeMs = SDL_GetTicks();
	}

};