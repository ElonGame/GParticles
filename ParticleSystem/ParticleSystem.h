#pragma once
#include <chrono>
#include <ratio>
#include "RendererProgram.h"
#include "ComputeProgram.h"
#include "Camera.h"

using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;
using ms = std::chrono::milliseconds;

class ParticleSystem
{
public:
	ParticleSystem(ComputeProgram &e, ComputeProgram &u, RendererProgram &r, glm::mat4 &m, unsigned int lif, bool lp)
		: emitter(e), updater(u), renderer(r), model(m), lifetime(ms(lif)), infinite(lif == 0), looping(lp) {};
	~ParticleSystem();

	void execute(Camera c);
	void printContents();
	bool isAlive();

	// emission
	timeClock::duration emissionStep = ms(100);
	timeP lastStep = timeClock::now();
	
private:
	ComputeProgram emitter;
	ComputeProgram updater;
	RendererProgram renderer;

	glm::mat4 model;

	// lifetime
	bool infinite = true;
	bool looping = false;
	timeClock::duration lifetime;
	timeP lifeStart;
	bool alive = true;
	bool firstExec = true;
};