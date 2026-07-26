#pragma once
#include <cstdint>
#include <cstddef>
#include <glm/glm.hpp>

namespace gpu {
	struct Particle {
		glm::vec4 position;// w is size
		glm::vec4 velocity;// w is burst flag
		glm::vec4 color; // Color
		float life; // Remaining life of the particle. if <0 : dead and unused.
		float maxLife; // Maximum life of the particle, used for fading out
		glm::vec2 padding;
	};

	struct EmitterConfig {
		glm::vec4 position;
		glm::vec4 direction;
		glm::vec4 startColor;
		glm::vec4 endColor;
		float size;
		float speed;
		float speedVariation; // Random variation in speed
		float spread;
		float particleLifetime;
		float particleLifetimeVariation; // Random variation in particle lifetime
		float spawnRate;
		int emitterType;
		float deltaTime;
		int burstCount;
		glm::vec2 padding;
	};

	struct ParticleCountData {
		uint32_t particleCount;
		uint32_t freeListHead;
	};

	// Size checks against the shader layouts.
	static_assert(sizeof(Particle) == 64, "gpu::Particle must match the std430 Particle in the compute shaders");
	static_assert(sizeof(EmitterConfig) == 112,	"gpu::EmitterConfig must match the std140 EmitterUniform block");

	// Offset checks: these are what the vertex-attribute pointers in main.cpp rely on.
	static_assert(offsetof(Particle, position) == 0, "Particle::position offset changed");
	static_assert(offsetof(Particle, velocity) == 16, "Particle::velocity offset changed");
	static_assert(offsetof(Particle, color) == 32, "Particle::color offset changed");
	static_assert(offsetof(Particle, life) == 48, "Particle::life offset changed");
	static_assert(offsetof(Particle, maxLife) == 52, "Particle::maxLife offset changed");

	// std140 offsets for the uniform block.
	static_assert(offsetof(EmitterConfig, spawnRate) == 88, "EmitterConfig::spawnRate offset changed");
	static_assert(offsetof(EmitterConfig, emitterType) == 92, "EmitterConfig::emitterType offset changed");
	static_assert(offsetof(EmitterConfig, deltaTime) == 96, "EmitterConfig::deltaTime offset changed");
	static_assert(offsetof(EmitterConfig, burstCount) == 100, "EmitterConfig::burstCount offset changed");
}