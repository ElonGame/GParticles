#pragma once
#include "GP_ParticleSystem.h"
#include "GP_Loader.h"

#define GPARTICLES GP_Systems::get()

class GP_Systems
{
public:
	static GP_Systems& get();
	

	std::vector<GP_ParticleSystem> pSystems;

	void loadProject(std::string filePath);
	void processParticles(glm::mat4 model = glm::mat4(), glm::mat4 view = glm::mat4(),
							glm::mat4 projection = glm::mat4());

private:
	GP_Systems() {};
};

