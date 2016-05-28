#include "GPManager.h"


GPManager::GPManager()
{
}


GPManager::~GPManager()
{
}

void GPManager::processParticles(glm::mat4 view)
{
	for (int i = 0; i < pSystems.size(); i++)
	{
		if (pSystems[i].isAlive())
		{
			pSystems[i].execute(view);
		}
		else
		{
			std::swap(pSystems[i], pSystems.back());
			pSystems.pop_back();
		}
	}
}

