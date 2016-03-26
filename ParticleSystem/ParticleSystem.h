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
	ParticleSystem(ComputeProgram &e, ComputeProgram &u, RendererProgram &r, glm::mat4 &m, unsigned int lif) : emitter(e), updater(u), renderer(r), model(m), lifetime(ms(lif)) {};
	ParticleSystem();
	~ParticleSystem();

	void execute(Camera c);
	void printContents();

	// emission
	timeClock::duration emissionStep = ms(100);
	timeP lastStep = timeClock::now();
	
private:
	ComputeProgram emitter;
	ComputeProgram updater;
	RendererProgram renderer;

	glm::mat4 model;
	// TODO: deal one leve above psLifetime
	timeClock::duration lifetime;
	timeP lifeStart;
	bool dead = false;
	bool firstExec = true;
};