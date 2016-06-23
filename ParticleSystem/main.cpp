#include "Utils.h"
#include "Window.h"
#include "GP_Loader.h"
#include "GPManager.h"
#include "Camera.h"
#include <IL\il.h>
#include <math.h>
#pragma comment(lib,"devil.lib")
#include "Model.h"

Camera c;
std::vector<Texture> g_textures;

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

	GPARTICLES.setUniformValue("mouseXY", glm::vec2(c.mouseX, c.mouseY));

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

	GPARTICLES.setWindowDimensions(1024, 576);

	// init opengl stuff
	Utils::initGL();

	ilInit();

	// init camera
	c = Camera();

	// init GPManager and load xml project file
	GPManager gpManager;
	//GP_Loader::loadProject("shaders/fireworks/fireworks.xml", gpManager.pSystems);
	//GP_Loader::loadProject("projects/virusPuddles/_virusPuddles.xml", gpManager.pSystems);
	//GP_Loader::loadProject("shaders/boids/boids.xml", gpManager.pSystems);
	GP_Loader::loadProject("projects/Tutorial_2/Tutorial_2.xml", gpManager.pSystems);



	GP_Uniform u;
	GPARTICLES.getUniform("virusAnimationAngle", u);
	float i = u.value[0].x;
	// system loop
	while (processEvents())
	{
		i += 0.01f;
		GPARTICLES.setUniformValue("virusAnimationAngle", ++i);

		// process all particle system
		gpManager.processParticles(c.getViewMatrix());

		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}

