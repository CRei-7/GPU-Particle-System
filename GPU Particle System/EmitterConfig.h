#pragma once
#include <glm/glm.hpp>

struct EmitterConfig{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec4 startColor;
	glm::vec4 endColor;
	float size;
	float speed;
	float speedVariation; // Random variation in speed
	float spread;
	float particleLifetime;
	float particleLifetimeVariation; // Random variation in particle lifetime
};
