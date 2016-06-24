#include "Utils.h"
#include "Window.h"
#include "Camera.h"
#include <math.h>
#pragma comment(lib,"devil.lib")
#include "Model.h"
#include "GP_Particles.h"

Camera c;

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
	// set new camera target only if left button is pressed
	if (SDL_GetMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		c.processMouseMovement();
	}

	GPDATA.setUniformValue("mouseXY", glm::vec2(c.mouseX, c.mouseY));

	return true;
}

int main(int argc, char* args[])
{
	//if (argc < 2)
	//{
	//	Utils::exitMessage("Invalid Input", "Please provide a project filePath");
	//}

	// init SDL
	Window window = Window();
	if (!window.init(1024, 576))
	{
		printf("Failed to initialize SDLWindow!\n");
		return 1;
	}

	// init opengl, devIL and set glViewPort
	Utils::init(1024, 576);

	GPDATA.setWindowDimensions(1024, 576);

	// init camera
	c = Camera();


	// load xml project file
	//GPARTICLES-loadProject("shaders/fireworks/fireworks.xml");
	//GPARTICLES.loadProject("shaders/boids/boids.xml");
	//GPARTICLES.loadProject("projects/Tutorial_2/Tutorial_2.xml");
	GPARTICLES.loadProject("projects/virusPuddles/_virusPuddles.xml");

	GP_Uniform u;
	GPDATA.getUniform("virusAnimationAngle", u);
	float i = u.value[0].x;
	// system loop
	while (processEvents())
	{
		i += 0.01f;
		GPDATA.setUniformValue("virusAnimationAngle", ++i);

		// process all particle system
		GPARTICLES.processParticles(c.getViewMatrix());

		window.swapWindow();
	}


	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}

