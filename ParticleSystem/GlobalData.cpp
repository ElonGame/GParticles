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

void GlobalData::setMouseXY(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void GlobalData::setWindowDimensions(float w, int h)
{
	windowWidth = w;
	windowHeight = h;
}

glm::vec2 GlobalData::getMouseXY(bool normalized)
{
	glm::vec2 res(mouseX, mouseY);

	if (normalized)
	{
		res.x /= windowWidth;
		res.y /= windowHeight;
	}

	return res;
}

