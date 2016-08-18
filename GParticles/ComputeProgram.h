#pragma once
#include "AbstractProgram.h"

class ComputeProgram : public AbstractProgram
{
public:
	ComputeProgram();
	ComputeProgram(const GLuint ph, const GLuint mp, const resHeader &a,
		const std::vector<std::string> &u, const std::string &psystemName,
		const GLuint iStep, const GLuint nwg, const resHeader &b)
		: AbstractProgram(ph, mp, a, u, psystemName, iStep), numWorkGroups(nwg), buffers(b)
	{
	};
	~ComputeProgram();

	void execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
	void printContents();

private:
	resHeader buffers;
	GLuint numWorkGroups;

	void bindResources();
};