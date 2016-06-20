#include "GlobalData.h"

GlobalData& GlobalData::get()
{
	static GlobalData instance;
	return instance;
}

int GlobalData::getCurrentTimeMillis()
{
	auto duration = timeClock::now().time_since_epoch();
	return std::chrono::duration_cast<ms>(duration).count();
}

void GlobalData::setWindowDimensions(float w, float h)
{
	windowWidth = w;
	windowHeight = h;
}

float GlobalData::getWindowWidth()
{
	return windowWidth;
}

float GlobalData::getWindowHeight()
{
	return windowHeight;
}

void GlobalData::addBuffer(GP_Buffer b)
{
	buffers[b.name] = b;
}

void GlobalData::addAtomic(GP_Atomic a)
{
	atomics[a.name] = a;
}

void GlobalData::addUniform(GP_Uniform u)
{
	uniforms[u.name] = u;
}

bool GlobalData::getBuffer(std::string name, GP_Buffer & b)
{
	if (buffers.find(name) == buffers.end() || buffers.empty())
		return false;

	b = buffers.at(name);
	return true;
}

bool GlobalData::getAtomic(std::string name, GP_Atomic & a)
{
	if (atomics.find(name) == atomics.end() || atomics.empty())
		return false;

	a = atomics.at(name);
	return true;
}

bool GlobalData::getUniform(std::string name, GP_Uniform &u)
{
	if (uniforms.find(name) == uniforms.end() || uniforms.empty())
		return false;

	u = uniforms.at(name);
	return true;
}

bufferUmap GlobalData::getBufferMap()
{
	return buffers;
}

atomicUmap GlobalData::getAtomicMap()
{
	return atomics;
}

uniformUmap GlobalData::getUniformMap()
{
	return uniforms;
}
