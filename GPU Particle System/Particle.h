#pragma once
#include <glm/glm.hpp>

#ifdef r
#undef r
#endif
#ifdef g
#undef g
#endif
#ifdef b
#undef b
#endif
#ifdef a
#undef a
#endif

struct Particle {
	glm::vec4 position;// z is size
	glm::vec4 velocity;// z is used as a dummy data
	glm::vec4 color; // Color
	float life; // Remaining life of the particle. if <0 : dead and unused.
	//float size;
	//float angle;
	//float weight;
	float maxLife; // Maximum life of the particle, used for fading out

	float NormalizedLife() const {
		return (maxLife > 0.0f) ? life / maxLife : 0.0f; // Returns a value between 0 and 1 representing the remaining life percentage
	}
};
