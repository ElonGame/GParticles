#pragma once
#include <glew\glew.h>
#include <iostream>
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
	}
}