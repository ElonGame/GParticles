#pragma once
#include <sdl2\glew.h>
#include <gl\GLU.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <chrono>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "PipelineUtil.h"

using ns = std::chrono::milliseconds;
using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;

class Pipeline
{
public:
	// glm stuff
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
	const int numParticles = 512;
	const int workGroupSize = 4;
	// atomic counters (first 8 binding points reserved)
	GLuint aliveParticles = 0;
	GLuint emissionAttempts = 0;
	GLuint created = 0;
	// buffer handles
	GLuint indexSSBO = 0;
	GLuint lifeSSBO = 0;
	GLuint posSSBO = 0;
	GLuint velSSBO = 0;
	// program handles
	GLuint emitterHandle = 0;
	GLuint updaterHandle = 0;
	GLuint rendererHandle = 0;
	GLuint vao = 0;

	// emission globals
	ns emissionStep = ns(10);
	timeP lastStep = timeClock::now();
	timeP emissionLoopStart;

	Pipeline();

	bool initBuffers();
	bool initEmitterProgram();
	bool initUpdaterProgram();
	bool initRendererProgram();
	void emit();
	void update();
	void render();
};

