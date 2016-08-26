#include "GP_Systems.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GP_Systems& GP_Systems::getSingleton()
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
}

void GP_Systems::addPSystem(GP_ParticleSystem &ps)
{
	pSystems.push_back(ps);
}

bool GP_Systems::getPSystem(std::string name, GP_ParticleSystem & ps)
{
	for (auto p : pSystems)
	{
		if (p.getName() == name)
		{
			ps = p;
			return true;
		}
	}

	return false;
}

bool GP_Systems::getPSystemStage(std::string psystemName, AbstractStage *&stage, std::vector<std::string> tags)
{
	GP_ParticleSystem ps;
	if (!getPSystem(psystemName, ps))
		return false;

	return ps.getStage(stage, tags);
}

void GP_Systems::loadProject(std::string filePath)
{
	GP_Loader::loadProject(filePath);
}
