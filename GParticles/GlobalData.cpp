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


bool GlobalData::setUniformValue(std::string name, float value)
{
	GP_Uniform u;
	getUniform(name, u);

	if (u.type == "uint" && value >= 0 || u.type == "float")
	{
		u.value[0].x = value;
	}
	else
	{
		return false;
	}
	
	addUniform(u);

	return true;
}

bool GlobalData::setUniformValue(std::string name, glm::vec2 value)
{
	GP_Uniform u;
	getUniform(name, u);

	if (u.type != "vec2")
		return false;

	u.value[0].x = value.x;
	u.value[0].y = value.y;
	addUniform(u);

	return true;
}

bool GlobalData::setUniformValue(std::string name, glm::vec4 value)
{
	GP_Uniform u;
	getUniform(name, u);

	if (u.type != "vec4")
		return false;

	u.value[0] = value;
	addUniform(u);

	return true;
}

bool GlobalData::setUniformValue(std::string name, glm::mat4 value)
{
	GP_Uniform u;
	getUniform(name, u);

	if (u.type != "mat4")
		return false;

	u.value = value;
	addUniform(u);

	return true;
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
