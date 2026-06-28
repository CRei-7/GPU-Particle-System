#pragma once
#include "Emitter.h"

class BurstEmitter : public Emitter
{
public:
	BurstEmitter(EmitterConfig emitter_config_, int count_);

	void update(float deltaTime, ParticlePool& pool) override;

	void spawnBurst(ParticlePool& pool, glm::vec3 position);

private:
	void spawnParticle(ParticlePool& pool) override;

	void writeParticle(Particle& p, glm::vec3 velocity) override;

	void spawnChildParticle(ParticlePool& pool, glm::vec3 position);

	int count;
	float spawnAccumulator;
	float spawnRate; //number of particles that shoot up per sec, making it different from continuous
};

