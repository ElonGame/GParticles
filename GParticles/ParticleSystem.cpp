#include "ParticleSystem.h"


ParticleSystem::~ParticleSystem()
{
}


void ParticleSystem::execute(glm::mat4 view)
{
	if (firstExec)
	{
		lifeStart = GlobalData::get().getCurrentTimeMillis();
		firstExec = false;
	}

	GLuint currTime = GlobalData::get().getCurrentTimeMillis();
	auto age = currTime - lifeStart;
	if (age > lifetime)
	{
		if (looping)
			lifeStart = currTime;
		else
			alive = false;
	}

	emitter.execute(numWorkGroups);
	updater.execute(numWorkGroups);
	renderer.execute(model, view);
}

void ParticleSystem::printContents()
{
	emitter.printContents();
	updater.printContents();
	renderer.printContents();
}

bool ParticleSystem::isAlive()
{
	return alive;
}
