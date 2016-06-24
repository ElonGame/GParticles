#include "GP_Particles.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GP_Particles& GP_Particles::get()
{
	static GP_Particles instance;
	return instance;
}

void GP_Particles::processParticles(glm::mat4 view)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < pSystems.size(); i++)
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

	glUseProgram(NULL);
}

void GP_Particles::loadProject(std::string filePath)
{
	GP_Loader::loadProject(filePath);
}
