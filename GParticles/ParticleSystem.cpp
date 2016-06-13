#include "ParticleSystem.h"


ParticleSystem::~ParticleSystem()
{
}

//Criar função canExecute
//

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

	//std::cout << "Time: " << std::chrono::duration_cast<ms>(timeSpan).count() << std::endl;
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
