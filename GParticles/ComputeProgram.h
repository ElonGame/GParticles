#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Utils.h"
#include "GP_Data.h"

class ComputeProgram
{
public:
	ComputeProgram(	GLuint ph, const resHeader &b, const resHeader &a, const std::vector<std::string> &u,
					const std::string psystemName, GLuint &mp, GLuint iStep)
		: programhandle(ph), buffers(b), atomics(a), uniforms(u), psystem(psystemName), maxParticles(mp), iterationStep(iStep), firstExec(true) {};
	ComputeProgram();
	~ComputeProgram();

	void execute(GLuint numWorkGroups);
	void printContents();


private:
	GLuint programhandle;
	resHeader buffers;
	resHeader atomics;
	std::vector<std::string> uniforms;
	std::string psystem;
	GLuint maxParticles;
	GLuint iterationStep;
	GLuint lastStep;
	bool canKeepUpIteration;
	bool firstExec;

	void bindResources();
};