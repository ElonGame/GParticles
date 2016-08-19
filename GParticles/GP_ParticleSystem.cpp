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


	for (auto p : programs)
	{
		p->execute(newModel, view, projection);
	}

	//emitter->execute(newModel, view, projection);
	//updater->execute(newModel, view, projection);
	//renderer->execute(newModel, view, projection);
}

void GP_ParticleSystem::printContents()
{
	for (auto p : programs)
	{
		p->printContents();
	}
	/*emitter->printContents();
	updater->printContents();
	renderer->printContents();*/
}

bool GP_ParticleSystem::isAlive()
{
	return alive;
}
