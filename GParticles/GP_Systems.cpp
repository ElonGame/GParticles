#include "GP_Systems.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GP_Systems& GP_Systems::get()
{
	static GP_Systems instance;
	return instance;
}

void GP_Systems::processParticles(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < pSystems.size(); i++)
	{
		if (pSystems[i].isAlive())
		{
			pSystems[i].execute(model, view, projection);
		}
		else
		{
			std::swap(pSystems[i], pSystems.back());
			pSystems.pop_back();
		}
	}

	glUseProgram(NULL);
}

void GP_Systems::loadProject(std::string filePath)
{
	GP_Loader::loadProject(filePath);
}
