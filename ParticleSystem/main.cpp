#include "Utils.h"
#include "Window.h"
#include "ParticleSystemLoader.h"
#include "ParticleSystemManager.h"
#include "Camera.h"
#include <IL\il.h>
#pragma comment(lib,"devil.lib")

Camera c;

void fetchParticleSystems(ParticleSystemManager &psm)
{
	// TODO: unbind current state and delete everything

	std::vector<ParticleSystem> psContainer;
	ParticleSystemLoader::loadProject("shaders/fireworks/userInput.xml", psContainer);

	psm.setContainer(psContainer);
}


bool processEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
			return false;
	}

	// process keys being held
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if (keystate[SDL_SCANCODE_A])
	{
		c.processKeyboard(Camera_Movement::LEFT);
	}
	if (keystate[SDL_SCANCODE_D])
	{
		c.processKeyboard(Camera_Movement::RIGHT);
	}
	if (keystate[SDL_SCANCODE_W])
	{
		c.processKeyboard(Camera_Movement::FORWARD);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		c.processKeyboard(Camera_Movement::BACKWARD);
	}

	// process mouse input and position
	c.lastMouseX = c.mouseX;
	c.lastMouseY = c.mouseY;
	int mouseX, mouseY;
	// set new camera target only if left button is pressed
	if (SDL_GetMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		c.processMouseMovement();
	}
	
	return true;
}


int main(int argc, char* args[])
{
	// init SDL
	Window window = Window();
	if (!window.init(1024, 576))
	{
		printf("Failed to initialize SDLWindow!\n");
		return 1;
	}

	// init opengl stuff
	Utils::initGL();

	ilInit();

	// init camera
	c = Camera();

	// init ParticleSystemManager and load particle systems
	ParticleSystemManager psManager = ParticleSystemManager();
	fetchParticleSystems(psManager);

	// system loop
	while (processEvents())
	{
		// TODO: system ticks here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// process all particle system
		psManager.processParticles(c);

		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}