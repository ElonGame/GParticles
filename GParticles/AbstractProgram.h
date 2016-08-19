#pragma once
#include "Utils.h"
#include "GP_Data.h"

class AbstractProgram
{
public:
	AbstractProgram();
	AbstractProgram(const GLuint ph, const GLuint mp, const resHeader &a,
		const std::vector<std::string> &u, const std::string &psystemName,
		const std::vector<std::string> &tgs, const GLuint iStep)
		: pHandle(ph), maxParticles(mp), atomics(a), uniforms(u), tags(tgs),
		psystem(psystemName), iterationStep(iStep), firstExec(true) {};
	virtual ~AbstractProgram();

	virtual void execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) = 0;
	virtual void printContents() = 0;
	void resetMarkedAtomics();

protected:
	GLuint pHandle;
	GLuint maxParticles;
	resHeader atomics;
	std::vector<std::string> uniforms;
	std::string psystem;
	std::vector<std::string> tags;
	GLuint iterationStep;
	GLuint lastStep;
	bool canKeepUpIteration;
	bool firstExec;

	void bindResources();
};