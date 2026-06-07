#pragma once
#include "ParticlePool.h"

class Emitter
{
public:
	Emitter(glm::vec3 position_, glm::vec3 direction_,
		float spawnRate_, float speed_, float minSpread_, float maxSpread_, float particleLifetime);

	void update(float deltaTime, ParticlePool &pool);

	void setPosition(const glm::vec3& newPosition);
	void setDirection(const glm::vec3& newDirection);
	void setSpawnRate(float newSpawnRate);
	void setSpeed(float newSpeed);
	void setSpread(float newMinSpread, float newMaxSpread);
	void setParticleLifetime(float newLifetime);

	~Emitter();

private:
	void spawnParticle(ParticlePool& pool);//Spawns a single particle using the emitter's properties and adds it to the pool

	glm::vec3 position;
	glm::vec3 direction;
	float spawnRate; // Particles per second
	float speed;
	float minSpread; // Angle in degrees for random spread
	float maxSpread;
	float particleLifetime;
	float accumulator; // Accumulates time to determine when to spawn the next particle
};

