#include "ParticleSystemManager.h"



ParticleSystemManager::ParticleSystemManager()
{
}


ParticleSystemManager::~ParticleSystemManager()
{
}

void ParticleSystemManager::processParticles(Camera &c)
{
	for (int i = 0; i < psContainer.size(); i++)
	{
		if (psContainer[i].isAlive())
		{
			psContainer[i].execute(c);
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

