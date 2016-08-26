#pragma once
#include "AbstractStage.h"

class ComputeStage : public AbstractStage
{
public:
	ComputeStage();
	ComputeStage(const GLuint ph, const GLuint mp, const resHeader &a,
		const std::vector<std::string> &u, const std::string &psystemName,
		const std::set<std::string> &tgs, const GLuint iStep, const GLuint nwg,
		const resHeader &b)
		: AbstractStage(ph, mp, a, u, psystemName, tgs, iStep), numWorkGroups(nwg), buffers(b)
	{
	};
	~ComputeStage();

	void execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
	void printContents();

private:
	resHeader buffers;
	GLuint numWorkGroups;

	void bindResources();
};