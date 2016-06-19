#pragma once
#include <glew\glew.h>
#include <string>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>


class GP_Uniform
{
public:
	std::string name;
	std::string type;
	glm::mat4 value;

	GP_Uniform();
	~GP_Uniform();

	void bind(GLint location);
	bool setValue(float value);
	bool setValue(glm::vec2 value);
	bool setValue(glm::vec4 value);
	bool setValue(glm::mat4 value);
};

