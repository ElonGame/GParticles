#pragma once
#include "RendererProgram.h"
#include "ComputeProgram.h"

using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;
using ms = std::chrono::milliseconds;

class ParticleSystem
{
public:
	ParticleSystem(ComputeProgram &e, ComputeProgram &u, RendererProgram &r, glm::mat4 &m, GLuint nwg, unsigned int lif, bool lp)
		: emitter(e), updater(u), renderer(r), model(m), numWorkGroups(nwg), lifetime(ms(lif)), infinite(lif == 0), looping(lp) {};
	~ParticleSystem();

	void execute(glm::mat4 view);
	void printContents();
	bool isAlive();

private:
	ComputeProgram emitter;
	ComputeProgram updater;
	RendererProgram renderer;

	glm::mat4 model;

	GLuint numWorkGroups;

	// lifetime
	bool infinite = true;
	bool looping = false;
	timeClock::duration lifetime;
	timeP lifeStart;
	bool alive = true;
	bool firstExec = true;
};