#pragma once
#include "RenderStage.h"
#include "ComputeStage.h"

class GP_ParticleSystem
{
public:
	GP_ParticleSystem(std::string n, std::vector<AbstractStage *> &stgs,
		glm::mat4 &m, GLuint nwg, GLuint lif, bool lp)
		: name(n), stages(stgs), psModel(m), numWorkGroups(nwg), lifetime(lif),
		infinite(lif == 0), looping(lp) {};
	GP_ParticleSystem() : name("Unnamed") {};
	~GP_ParticleSystem();

	void execute(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
	void printContents();
	bool isAlive();
	std::string getName();
	bool getStage(AbstractStage *&stage, std::vector<std::string> tags = std::vector<std::string>());

private:
	std::string name;
	std::vector<AbstractStage *> stages;
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