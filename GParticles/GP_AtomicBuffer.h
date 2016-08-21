#pragma once
#include <GL\glew.h>
#include <string>
#include <unordered_map>


struct GP_Atomic
{
	GLuint offset;
	std::string name;
	GLuint resetValue;
	bool reset;

	GP_Atomic() : offset(-1), name("Unnamed"), resetValue(0), reset(false) {};
};

class GP_AtomicBuffer
{
public:
	GLuint id;
	std::string name;
	std::unordered_map<std::string, GP_Atomic> atomics;

	GP_AtomicBuffer() : id(0), name("Unnamed") {};
	GP_AtomicBuffer(std::string n) : id(0), name(n) {};
	GP_AtomicBuffer(std::string n, std::unordered_map<std::string, GP_Atomic> a)
		: id(0), name(n), atomics(a)
	{
		init();
	};
	~GP_AtomicBuffer();

	void init();
	void bind(GLuint bindingPoint);
	//GP_Atomic getAtomic(std::string name);
	//GLuint getAtomicValue(GLuint offset);
	//void setAtomicValue(GLuint value);
};
