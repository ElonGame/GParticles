#include "GP_ParticleSystem.h"


GP_ParticleSystem::~GP_ParticleSystem()
{
}


void GP_ParticleSystem::execute(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	if (firstExec)
	{
		lifeStart = GP_Data::get().getCurrentTimeMillis();
		firstExec = false;
	}

	GLuint currTime = GP_Data::get().getCurrentTimeMillis();
	auto age = currTime - lifeStart;
	if (age > lifetime)
	{
		if (looping)
			lifeStart = currTime;
		else
			alive = false;
	}

	glm::mat4 newModel = model * psModel;


	for (auto s : stages)
	{
		if ( ! s->hasTag("paused") )
		{
			s->execute(newModel, view, projection);
		}
	}
}

void GP_ParticleSystem::printContents()
{
	for (auto s : stages)
	{
		s->printContents();
	}
}

bool GP_ParticleSystem::isAlive()
{
	return alive;
}

std::string GP_ParticleSystem::getName()
{
	return name;
}

bool GP_ParticleSystem::getStage(AbstractStage *&stage, std::vector<std::string> tags)
{
	for (auto s : stages)
	{
		bool candidate = true;
		for (size_t i = 0; candidate && i < tags.size(); i++)
		{
			if (!s->hasTag(tags[i]))
			{
				candidate = false;
			}
		}

		if (candidate)
		{
			stage = s;
			return true;
		}
	}

	return false;
}
