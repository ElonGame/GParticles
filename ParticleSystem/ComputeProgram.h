#pragma once
#include <glew\glew.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
#include "Utils.h"

class ComputeProgram
{
public:
	ComputeProgram(GLuint ph, const std::vector<GLuint> &ah, const std::vector<GLuint> &bh, const uniformUmap &u)
		: programhandle(ph), atomicHandles(ah), bufferHandles(bh), uniforms(u) {};
	ComputeProgram();
	~ComputeProgram();

	void execute();
	void printContents();
private:
	GLuint programhandle;
	std::vector<GLuint> atomicHandles;
	uniformUmap uniforms;
	std::vector<GLuint> bufferHandles;

	void bindResources();
	void setUniforms();
};