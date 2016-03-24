#include "Utils.h"
#include "Window.h"
#include "ParticleSystemLoader.h"
#include "Camera.h"

// TODO: should this be here?
float mSpeed = 0.01f;
Camera c;
ParticleSystem currentPS;


ParticleSystem fetchParticleSystem()
{
	// TODO: unbind current state and delete everything

	ParticleSystemLoader psLoader = ParticleSystemLoader();
	ParticleSystem ps = psLoader.loadParticleSystem("shaders/userInput.xml");
	ps.printContents();
	return ps;
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
	if (keystate[SDL_SCANCODE_R])
	{
		fetchParticleSystem();
	}

	// process mouse input and position
	// set new camera target only if left button is pressed
	c.lastMouseX = c.mouseX;
	c.lastMouseY = c.mouseY;
	int mouseX, mouseY;
	if (SDL_GetGlobalMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		//std::cout << "last: " << c.lastMouseX << ", " << c.lastMouseY << " --- current: " << c.mouseX << ", " << c.mouseY << std::endl;

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

	// init ParticleSystem
	currentPS = fetchParticleSystem();


	// system loop
	while (processInput())
	{
		currentPS.execute(c);

		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}