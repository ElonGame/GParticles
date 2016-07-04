#pragma once
#include <GL\glew.h>
#include <IL\il.h>
#include <sdl2\SDL.h>
#include <unordered_map>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "GP_Atomic.h"
#include "GP_Uniform.h"
#include "GP_Buffer.h"

using resHeader = std::vector<std::pair<std::string, GLuint>>;
using bufferUmap = std::unordered_map<std::string, GP_Buffer>;
using atomicUmap = std::unordered_map<std::string, GP_Atomic>;
using uniformUmap = std::unordered_map<std::string, GP_Uniform>;

namespace Utils
{
	inline void init()
	{
		// init GLEW
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glClearColor(0.2f, 0.1f, 0.2f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// init devIL
		ilInit();
	}

	inline void exitMessage(std::string title, std::string msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), NULL);
		exit(0);
	}
}