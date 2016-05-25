#include "ParticleSystem.h"


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::execute(glm::mat4 view)
{
	if (!alive)
		return;

	if (firstExec)
	{
		lifeStart = timeClock::now();
		firstExec = false;
	}

	timeP currTime = timeClock::now();
	auto age = currTime - lifeStart;
	if (age > lifetime)
		if (looping)
			lifeStart = currTime;
		else
			alive = false;


	//std::cout << "Time: " << std::chrono::duration_cast<ms>(age).count() << std::endl;

	auto timeSpan = currTime - lastStep;

	if (timeSpan > emissionStep)
	{
		emitter.execute(numWorkGroups);

		lastStep += emissionStep;
	}
	//std::cout << "Time: " << std::chrono::duration_cast<ms>(timeSpan).count() << std::endl;
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
