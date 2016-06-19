#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Utils.h"
#include "GlobalData.h"

class ComputeProgram
{
public:
	ComputeProgram(	GLuint ph, const std::vector<std::string> &ah, const bufferUmap &bh,
					const std::vector<std::string> &u, GLuint &mp, GLuint iStep)
		: programhandle(ph), atomics(ah), buffers(bh), uniforms(u), maxParticles(mp), iterationStep(ms(iStep)) {};
	ComputeProgram();
	~ComputeProgram();

	void execute(GLuint numWorkGroups);
	void printContents();

	timeClock::duration iterationStep;
	timeP lastStep = timeClock::now();
private:
	GLuint programhandle;
	std::vector<std::string> atomics;
	std::vector<std::string> uniforms;
	bufferUmap buffers;

	GLuint maxParticles;

	void bindResources();
};