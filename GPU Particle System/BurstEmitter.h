#pragma once
#include "Emitter.h"

class BurstEmitter : public Emitter
{
public:
	BurstEmitter(EmitterConfig emitter_config_, int count_);

	void update(float deltaTime, ParticlePool& pool) override;

	void trigger() { triggered = true; }

	bool isTriggered() { return triggered; }

private:
	void spawnParticle(ParticlePool& pool) override;

	int count;

	bool triggered = false;
};

