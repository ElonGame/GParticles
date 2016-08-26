#pragma once
#include "GP_ParticleSystem.h"
#include "GP_Loader.h"

#define GPSYSTEMS GP_Systems::getSingleton()

class GP_Systems
{
public:
	static GP_Systems& getSingleton();
	
	void loadProject(std::string filePath);
	void processParticles(glm::mat4 model = glm::mat4(), glm::mat4 view = glm::mat4(),
							glm::mat4 projection = glm::mat4());
	void addPSystem(GP_ParticleSystem &ps);
	bool getPSystem(std::string name, GP_ParticleSystem &ps);
	bool getPSystemStage(std::string psystemName, AbstractStage *&stage, std::vector<std::string> tags = std::vector<std::string>());

private:
	std::vector<GP_ParticleSystem> pSystems;

	GP_Systems() {};
};

