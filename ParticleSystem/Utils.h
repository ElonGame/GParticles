#pragma once
#include <glew\glew.h>
#include <unordered_map>

struct bufferInfo
{
	GLuint id;
	GLuint elements;
	std::string type;
	std::string name;
	GLuint binding;
};

struct atomicInfo
{
	GLuint id;
	GLuint initialValue;
	std::string name;
	GLuint binding;
};

struct uniformInfo
{
	GLfloat value;
	std::string type;
	std::string name;
};

using bufferUmap = std::unordered_map<std::string, bufferInfo>;
using atomicUmap = std::unordered_map<std::string, atomicInfo>;
using uniformUmap = std::unordered_map<std::string, uniformInfo>;

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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}
}