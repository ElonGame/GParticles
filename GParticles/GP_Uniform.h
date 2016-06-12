#pragma once
#include <glew\glew.h>
#include <string>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <map>


class GP_Uniform
{
public:
	std::string name;
	glm::mat4 value;
	std::string type;

	GP_Uniform();
	~GP_Uniform();

	void bind(GLint location);
};

