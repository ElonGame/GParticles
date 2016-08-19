#pragma once
#include "RendererProgram.h"
#include "ComputeProgram.h"

class GP_ParticleSystem
{
	friend class ComputeProgram;

public:
	GP_ParticleSystem(std::vector<AbstractProgram *> &pgs, glm::mat4 &m, GLuint nwg, GLuint lif, bool lp)
		: programs(pgs), psModel(m), numWorkGroups(nwg), lifetime(lif), infinite(lif == 0), looping(lp) {};
	~GP_ParticleSystem();

	void execute(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
	void printContents();
	bool isAlive();

private:
	std::vector<AbstractProgram *> programs;

	glm::mat4 psModel;

	GLuint numWorkGroups;

	// lifetime
	bool infinite = true;
	bool looping = false;
	GLuint lifetime;
	GLuint lifeStart;
	bool alive = true;
	bool firstExec = true;
};