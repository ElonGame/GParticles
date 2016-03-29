#include "Utils.h"
#include "Window.h"
#include "ParticleSystemLoader.h"
#include "ParticleSystemManager.h"
#include "Camera.h"

// TODO: should this be here?
float mSpeed = 0.01f;
Camera c;

void fetchParticleSystems(ParticleSystemManager &psm)
{
	// TODO: unbind current state and delete everything
	std::vector<ParticleSystem> psContainer;
	ParticleSystemLoader::loadProject("shaders/userInput.xml", psContainer);
	psm.setContainer(psContainer);
}


bool processInput()
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
		c.processKeyboard(Camera_Movement::LEFT, mSpeed);
	}
	if (keystate[SDL_SCANCODE_D])
	{
		c.processKeyboard(Camera_Movement::RIGHT, mSpeed);
	}
	if (keystate[SDL_SCANCODE_W])
	{
		c.processKeyboard(Camera_Movement::FORWARD, mSpeed);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		c.processKeyboard(Camera_Movement::BACKWARD, mSpeed);
	}

	// process mouse input and position
	// set new camera target only if left button is pressed
	c.lastMouseX = c.mouseX;
	c.lastMouseY = c.mouseY;
	int mouseX, mouseY;
	if (SDL_GetGlobalMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
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

	// init glew and set viewport
	Utils::initGL();
	glViewport(0, 0, 1024, 576);

	// init camera
	c = Camera();

	// init ParticleSystemManager and load particle systems
	ParticleSystemManager psManager = ParticleSystemManager();
	fetchParticleSystems(psManager);

	// system loop
	while (processInput())
	{
		// TODO: system ticks here
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// process all particle system
		psManager.processParticles(c);

		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}