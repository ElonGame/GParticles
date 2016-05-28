#pragma once
#include "ParticleSystem.h"

class GPManager
{
public:
	std::vector<ParticleSystem> pSystems;

	GPManager();
	~GPManager();

	void processParticles(glm::mat4 view);
};

