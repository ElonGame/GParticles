#pragma once
#include "ParticleSystem.h"

class ParticleSystemManager
{
public:
	ParticleSystemManager();
	~ParticleSystemManager();

	void processParticles(glm::mat4 view);
	void setContainer(std::vector<ParticleSystem> &container);

private:
	std::vector<ParticleSystem> psContainer;
};

