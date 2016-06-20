#pragma once
#include <glew\glew.h>
#include <string>

class GP_Atomic
{
public:
	GLuint id;
	GLuint binding;
	std::string name;
	GLuint resetValue;
	bool reset;

	GP_Atomic() : id(-1), resetValue(0), reset(false) {};
	~GP_Atomic();

	void init();
	void bind(GLuint bindingPoint);
	GLuint getCurrentValue();
	void setCurrentValue(GLuint value);
};

