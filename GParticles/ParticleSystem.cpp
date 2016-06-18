#include "ParticleSystem.h"


ParticleSystem::~ParticleSystem()
{
}


void ParticleSystem::execute(glm::mat4 view)
{
	if (firstExec)
	{
		lifeStart = timeClock::now();
		firstExec = false;
	}

	timeP currTime = timeClock::now();
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
