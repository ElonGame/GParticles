#pragma once
#include "GP_PSystem.h"
#include "GP_Loader.h"

#define GPARTICLES GP_Particles::get()

class GP_Particles
{
public:
	static GP_Particles& get();
	

	std::vector<GP_PSystem> pSystems;

	void loadProject(std::string filePath);
	void processParticles(glm::mat4 view = glm::mat4());

private:
	GP_Particles() {};
};

