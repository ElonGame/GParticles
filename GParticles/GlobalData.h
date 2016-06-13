#pragma once
#include <chrono>
#include <ratio>
#include <iostream>
#include "Utils.h"

using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;
using ms = std::chrono::milliseconds;

class GlobalData
{
public:
	static GlobalData& get();

	int getCurrentTimeMillis();
	void setWindowDimensions(float w, float h);
	float getWindowWidth();
	float getWindowHeight();

	// resource functions
	//void addBuffer(bufferInfo b);
	//void addAtomic(atomicInfo a);
	void addUniform(GP_Uniform u);
	//bufferInfo getBuffer(std::string name);
	//atomicInfo getAtomic(std::string name);
	bool setUniformValue(std::string name, float value);
	bool setUniformValue(std::string name, glm::vec2 value);
	bool setUniformValue(std::string name, glm::vec4 value);
	bool setUniformValue(std::string name, glm::mat4 value);
	bool getUniform(std::string name, GP_Uniform &u);
	uniformUmap getUniformMap();


	GlobalData(GlobalData const&)		= delete;
	void operator=(GlobalData const&)	= delete;

private:
	float windowWidth;
	float windowHeight;

	// resources
	bufferUmap buffers;
	atomicUmap atomics;
	uniformUmap uniforms;

	GlobalData() {};
};

