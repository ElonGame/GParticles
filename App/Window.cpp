#include "Window.h"


Window::Window()
{
}


bool Window::init(int screenW, int screenH)
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// opengl version
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// create window
		window = SDL_CreateWindow(	"Particle System", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									screenW, screenH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// create context
			context = SDL_GL_CreateContext(window);
			if (context == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
		}
		// use VSYNC
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
	}

	return success;
}

void Window::setSize(int screenWidth, int screeHeight)
{
	SDL_SetWindowSize(window, screenWidth, screeHeight);
}

void Window::swapWindow()
{
	SDL_GL_SwapWindow(window);
}


void Window::quit()
{
	// destroy window	
	SDL_DestroyWindow(window);
	window = nullptr;

	// quit SDL subsystems
	SDL_Quit();
}