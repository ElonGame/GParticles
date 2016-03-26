#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::execute(Camera c)
{
	if (dead)
		return;

	if (firstExec)
	{
		lifeStart = timeClock::now();
		firstExec = false;
	}

	timeP currTime = timeClock::now();
	auto age = currTime - lifeStart;
	if (age > lifetime)
		dead = true;

	//std::cout << "LifeStart: " << std::chrono::time_point_cast<ms>(lifeStart).time_since_epoch().count() << std::endl;
	//std::cout << "curretime: " << std::chrono::time_point_cast<ms>(currTime).time_since_epoch().count() << std::endl;
	std::cout << "Time: " << std::chrono::duration_cast<ms>(age).count() << std::endl;

	auto timeSpan = currTime - lastStep;

	if (timeSpan > emissionStep)
	{
		emitter.execute();

		lastStep += emissionStep;
	}
	//std::cout << "Time: " << std::chrono::duration_cast<ms>(timeSpan).count() << std::endl;

	updater.execute();
	renderer.execute(model, c);
}

void ParticleSystem::printContents()
{
	emitter.printContents();
	updater.printContents();
	renderer.printContents();
}
