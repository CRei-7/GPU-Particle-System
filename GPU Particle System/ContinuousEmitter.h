#pragma once
#include "Emitter.h"
#include "EmitterConfig.h"

class ContinuousEmitter : public Emitter
{
public:
	ContinuousEmitter(EmitterConfig emitter_config, float spawnRate_);

	void update(float deltaTime, ParticlePool& pool) override; // Override the pure virtual function from the base class to provide specific behavior for continuous emission

private:
	float spawnRate;
	float accumulator; // Accumulates time to determine when to spawn the next particle
};

