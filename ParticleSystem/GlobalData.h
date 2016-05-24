#pragma once
#include <chrono>
#include <ratio>
#include <iostream>
#include <glm\gtc\matrix_transform.hpp>

using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;
using ms = std::chrono::milliseconds;

class GlobalData
{
public:
	static GlobalData& getInstance();

	int getCurrentTimeMillis();
	void setMouseXY(int x, int y);
	void setWindowDimensions(float w, int h);
	glm::vec2 getMouseXY(bool normalized);

	GlobalData(GlobalData const&)		= delete;
	void operator=(GlobalData const&)	= delete;

private:
	int mouseX;
	int mouseY;
	float windowWidth;
	float windowHeight;

	GlobalData() {};
};

