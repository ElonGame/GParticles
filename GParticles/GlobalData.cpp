#include "GlobalData.h"

GlobalData& GlobalData::getInstance()
{
	static GlobalData instance;
	return instance;
}

int GlobalData::getCurrentTimeMillis()
{
	auto duration = timeClock::now().time_since_epoch();
	return std::chrono::duration_cast<ms>(duration).count();
}

void GlobalData::setMouseXY(float x, float y)
{
	mouseX = x;
	mouseY = y;
}

void GlobalData::setWindowDimensions(float w, float h)
{
	windowWidth = w;
	windowHeight = h;
}

float GlobalData::getMouseX(bool normalized)
{
	return (normalized) ? mouseX / windowWidth : mouseX;
}

float GlobalData::getMouseY(bool normalized)
{
	return (normalized) ? mouseY / windowHeight : mouseY;
}

float GlobalData::getWindowWidth()
{
	return windowWidth;
}

float GlobalData::getWindowHeight()
{
	return windowHeight;
}