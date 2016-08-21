#pragma once
#include <chrono>
#include <ratio>
#include <iostream>
#include "Utils.h"

#define GPDATA GP_Data::get()

using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;
using ms = std::chrono::milliseconds;

class GP_Data
{
public:
	static GP_Data& get();


	unsigned int getCurrentTimeMillis();
	float getCurrentTimeSeconds();
	void setWindowDimensions(float w, float h);
	float getWindowWidth();
	float getWindowHeight();

	// resource functions
	void addBuffer(GP_Buffer b);
	void addAtomicBuffer(GP_AtomicBuffer &ab);
	void addUniform(GP_Uniform u);
	bool getBuffer(std::string name, GP_Buffer &b);
	bool getAtomicBuffer(std::string name, GP_AtomicBuffer &ab);
	bool getAtomicAtomicBuffer(std::string name, GP_AtomicBuffer &ab);
	bool getAtomic(std::string name, GP_Atomic &a);
	bool getUniform(std::string name, GP_Uniform &u);
	bufferUmap getBufferMap();
	//atomicUmap getAtomicMap();
	uniformUmap getUniformMap();

	template <typename T>
	bool setUniformValue(std::string name, T value)
	{
		GP_Uniform u;
		getUniform(name, u);

		u.setValue(value);

		addUniform(u);

		return true;
	}

	GP_Data(GP_Data const&)		= delete;
	void operator=(GP_Data const&)	= delete;

private:
	float windowWidth;
	float windowHeight;

	// resources
	bufferUmap buffers;
	std::unordered_map<std::string, std::string> atomicAtomicBufferMap;
	atomicBufferUmap atomicBuffers;
	uniformUmap uniforms;

	GP_Data() {};
};

