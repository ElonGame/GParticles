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
	ComputeProgram(GLuint ph, const std::vector<std::pair<GLuint, GLuint>> &ah, const std::vector<GLuint> &ahtr, const std::vector<std::pair<GLuint, GLuint>> &bh, const uniformUmap &u)
		: programhandle(ph), atomicHandles(ah), atomicsToReset(ahtr), bufferHandles(bh), uniforms(u) {};
	ComputeProgram();
	~ComputeProgram();

	void execute();
	void printContents();
private:
	GLuint programhandle;
	std::vector<std::pair<GLuint, GLuint>> atomicHandles;
	std::vector<GLuint> atomicsToReset;
	uniformUmap uniforms;
	std::vector<std::pair<GLuint, GLuint>> bufferHandles;

	void bindResources();
	void setUniforms();
};