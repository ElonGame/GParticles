#pragma once
#include <glew\glew.h>
#include <sdl2\SDL.h>
#include <unordered_map>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "GP_Uniform.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct rendererLoading
{
	std::vector<std::string> vsPath;
	std::vector<std::string> fgPath;
	std::vector<std::string> gmPath;
	std::string rendertype;
	std::string path;
};

struct bufferInfo
{
	GLuint id;
	GLuint binding;
	GLuint elements;
	std::string type;
	std::string name;
};

struct atomicInfo
{
	GLuint id;
	GLuint binding;
	std::string name;
	GLuint initialValue;
	bool reset;
};

/*struct uniformInfo
{
	std::string name;
	std::string type;
	GLfloat value;
	//GP
};*/

using bufferUmap = std::unordered_map<std::string, bufferInfo>;
using atomicUmap = std::unordered_map<std::string, atomicInfo>;
using uniformUmap = std::unordered_map<std::string, GP_Uniform>;

namespace Utils
{
	inline void initGL()
	{
		// init GLEW
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}

		// set viewport
		glViewport(0, 0, 1024, 576);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.2f, 0.1f, 0.2f, 1.0f);
	}

	inline void exitMessage(std::string title, std::string msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), NULL);
		exit(0);
	}
}