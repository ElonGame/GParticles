#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Utils.h"
#include "GlobalData.h"

class ComputeProgram
{
public:
	ComputeProgram(	GLuint ph, const atomicUmap &ah, const bufferUmap &bh,
					const std::vector<std::string> &u, GLuint &mp)
		: programhandle(ph), atomics(ah), buffers(bh), uniforms(u), maxParticles(mp) {};
	ComputeProgram();
	~ComputeProgram();

	void execute(GLuint numWorkGroups);
	void printContents();
private:
	GLuint programhandle;
	atomicUmap atomics;
	std::vector<std::string> uniforms;
	bufferUmap buffers;

	GLuint maxParticles;

	void bindResources();
};