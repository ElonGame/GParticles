#include "ParticleSystemManager.h"



ParticleSystemManager::ParticleSystemManager()
{
}


ParticleSystemManager::~ParticleSystemManager()
{
}

void ParticleSystemManager::processParticles(glm::mat4 view)
{
	for (int i = 0; i < psContainer.size(); i++)
	{
		if (psContainer[i].isAlive())
		{
			psContainer[i].execute(view);
		}
		else
		{
			std::swap(psContainer[i], psContainer.back());
			psContainer.pop_back();
		}
	}
}

void ParticleSystemManager::setContainer(std::vector<ParticleSystem>& container)
{
	psContainer = container;
}

