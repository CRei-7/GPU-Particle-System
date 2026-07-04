#pragma once
#include <glm/glm.hpp>

struct Particle {
	glm::vec4 position;// z is size
	glm::vec4 velocity;// z is used as a dummy data
	glm::vec4 color; // Color
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float maxLife; // Maximum life of the particle, used for fading out
	float padding[2];
};
