#pragma once
#include <vector>
#include "ParticleSystem.h"
#include "Camera.h"

class ParticleSystemManager
{
public:
	ParticleSystemManager();
	~ParticleSystemManager();

	void processParticles(Camera &c);
	void setContainer(std::vector<ParticleSystem> &container);

private:
	std::vector<ParticleSystem> psContainer;
};

