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
	ParticleSystem(ComputeProgram &e, ComputeProgram &u, RendererProgram &r) : emitter(e), updater(u), renderer(r) {};
	ParticleSystem();
	~ParticleSystem();

	void execute(Camera c);
	void printContents();

	// emission globals
	timeClock::duration emissionStep = ms(100);
	timeP lastStep = timeClock::now();
	timeP emissionLoopStart;

protected:
	ComputeProgram emitter;
	ComputeProgram updater;
	RendererProgram renderer;
};