#pragma once
#include <sdl2\SDL.h>
#include <sdl2\SDL_opengl.h>
#include <string>


class Window
{
public:
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;


	Window();

	bool init(int screenWidth, int screeHeight);
	void swapWindow();
	void quit();
};

