#pragma once
#include <GL\glew.h>
#include <string>

class GP_Buffer
{
public:
	GLuint id;
	GLuint binding;
	std::string name;
	std::string type;
	GLuint elements;

	GP_Buffer() : id(-1), name("UNASSIGNED"), type("NONE"), elements(0) {};
	~GP_Buffer();

	void init(GLuint maxParticles = 0);
	void bind(GLuint bindingPoint);
};

