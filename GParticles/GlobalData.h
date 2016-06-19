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
	void addAtomic(GP_Atomic a);
	void addUniform(GP_Uniform u);
	//bufferInfo getBuffer(std::string name);
	//atomicInfo getAtomic(std::string name);

	template <typename T>
	bool setUniformValue(std::string name, T value)
	{
		GP_Uniform u;
		getUniform(name, u);

		u.setValue(value);

		addUniform(u);

		return true;
	}

	bool getAtomic(std::string name, GP_Atomic &a);
	bool getUniform(std::string name, GP_Uniform &u);
	atomicUmap getAtomicMap();
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

