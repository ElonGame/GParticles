#include "PipelineUtil.h"
#include "Window.h"
#include "Pipeline.h"



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
	initGL();
	glViewport(0, 0, 1024, 576);

	// init pipeline
	Pipeline pipeline = Pipeline();

	// init buffers
	if (!pipeline.initBuffers())
	{
		printf("Unable to initialize buffers!\n");
		return 1;
	}

	// init emitter program
	if (!pipeline.initEmitterProgram())
	{
		printf("Unable to initialize emitter program!\n");
		return 1;
	}

	// init update program
	if (!pipeline.initUpdaterProgram())
	{
		printf("Unable to initialize updater program!\n");
		return 1;
	}

	// init render program
	if (!pipeline.initRendererProgram())
	{
		printf("Unable to initialize renderer program!\n");
		return 1;
	}

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

		pipeline.emit();

		int x, y;
		SDL_GetMouseState(&x, &y);

		pipeline.update((2.0*x / 1024.0f) - 1.0f, -((2.0*y / 576.0f) - 1.0f));
		pipeline.render();

		window.swapWindow();
	}

	// TODO: deallocate programs

	// close window and deallocate SDL
	window.quit();

	return 0;
}