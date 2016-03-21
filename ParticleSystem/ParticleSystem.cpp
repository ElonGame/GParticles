#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::execute()
{
	auto timeSpan = timeClock::now() - lastStep;

	if (timeSpan > emissionStep)
	{
		emitter.execute();

		lastStep += emissionStep;
	}
	//std::cout << "Time: " << std::chrono::duration_cast<ms>(timeSpan).count() << std::endl;

	updater.execute();
	renderer.execute();
}

void ParticleSystem::printContents()
{
	emitter.printContents();
	updater.printContents();
	renderer.printContents();
}
