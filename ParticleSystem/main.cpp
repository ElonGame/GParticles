#include "Utils.h"
#include "Window.h"
#include "ParticleSystemLoader.h"

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

	// init ParticleSystem
	ParticleSystemLoader psLoader = ParticleSystemLoader();
	ParticleSystem ps = psLoader.loadParticleSystem("shaders/userInput.xml");
	ps.printContents();

	// system loop
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		ps.execute();

		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}