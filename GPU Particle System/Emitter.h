#pragma once
#include "ParticlePool.h"
#include "EmitterConfig.h"

class Emitter
{
public:
	explicit Emitter(EmitterConfig emitter_config_);

	virtual void update(float deltaTime, ParticlePool& pool) = 0; // Pure virtual function to be implemented by derived classes, this allows for different types of emitters with different update behaviors

	void setPosition(const glm::vec3& newPosition);
	void setDirection(const glm::vec3& newDirection);
	void setSpeed(float newSpeed);
	void setSpeedVariation(float newSpeedVariation);
	void setSpread(float newMSpread);
	void setParticleLifetime(float newLifetime);
	void setParticleLifetimeVariation(float newLifetimeVariation);
	void setStartColor(const glm::vec4& color);
	void setEndColor(const glm::vec4& color);
	void setSize(float newSize);

	const EmitterConfig& getConfig() const;

	virtual ~Emitter() = default; // Virtual destructor to allow proper cleanup in derived classes

protected:
	virtual void spawnParticle(ParticlePool& pool);//Spawns a single particle using the emitter's properties and adds it to the pool

	void writeParticle(Particle& p, glm::vec3 velocity);

	EmitterConfig emitter_config; // Store the emitter configuration for easy access in derived classes
};
