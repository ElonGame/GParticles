#include "GP_PSystem.h"


GP_PSystem::~GP_PSystem()
{
}


void GP_PSystem::execute(glm::mat4 view)
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

	emitter.execute(numWorkGroups);
	updater.execute(numWorkGroups);
	renderer.execute(model, view);
}

void GP_PSystem::printContents()
{
	emitter.printContents();
	updater.printContents();
	renderer.printContents();
}

bool GP_PSystem::isAlive()
{
	return alive;
}
