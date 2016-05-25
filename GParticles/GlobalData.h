#pragma once
#include <chrono>
#include <ratio>
#include <iostream>

using timeClock = std::chrono::steady_clock;
using timeP = std::chrono::time_point<timeClock>;
using ms = std::chrono::milliseconds;

class GlobalData
{
public:
	static GlobalData& getInstance();

	int getCurrentTimeMillis();
	void setMouseXY(float x, float y);
	void setWindowDimensions(float w, float h);
	float getMouseX(bool normalized);
	float getMouseY(bool normalized);
	float getWindowWidth();
	float getWindowHeight();

	GlobalData(GlobalData const&)		= delete;
	void operator=(GlobalData const&)	= delete;

private:
	float mouseX;
	float mouseY;
	float windowWidth;
	float windowHeight;

	GlobalData() {};
};

