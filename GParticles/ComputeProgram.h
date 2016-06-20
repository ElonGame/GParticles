#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Utils.h"
#include "GlobalData.h"

class ComputeProgram
{
public:
	ComputeProgram( GLuint ph, const resHeader &b, const resHeader &a,
					const std::vector<std::string> &u, GLuint &mp, GLuint iStep)
		: programhandle(ph), buffers(b), atomics(a), uniforms(u), maxParticles(mp), iterationStep(ms(iStep)) {};
	ComputeProgram();
	~ComputeProgram();

	void execute(GLuint numWorkGroups);
	void printContents();

	timeClock::duration iterationStep;
	timeP lastStep = timeClock::now();
private:
	GLuint programhandle;
	resHeader buffers;
	resHeader atomics;
	std::vector<std::string> uniforms;

	GLuint maxParticles;

	void bindResources();
};