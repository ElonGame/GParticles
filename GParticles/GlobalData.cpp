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

void GlobalData::addUniform(GP_Uniform u)
{
	uniforms[u.name] = u;
}

bool GlobalData::getUniform(std::string name, GP_Uniform &u)
{
	if (uniforms.find(name) == uniforms.end() || uniforms.empty())
		return false;

	u = uniforms.at(name);
	return true;
}

uniformUmap GlobalData::getUniformMap()
{
	return uniforms;
}
