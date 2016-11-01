#include "Utils.h"
#include "Window.h"
#include "Camera.h"
#include <math.h>
#pragma comment(lib,"devil.lib")
#include "GP_Systems.h"

Camera c;
Window window;

bool processEvents();
void setStageStubs();

int main(int argc, char* args[])
{
	// init SDL
	window = Window();
	if (!window.init(1024, 576))
	{
		printf("Failed to initialize SDLWindow!\n");
		return 1;
	}

	// init opengl, devIL and set glViewPort
	Utils::init();

	// set 
	GPDATA.setWindowDimensions(1024, 576);
	glm::mat4 projection = glm::perspective(45.0f, 1024/576.0f, 0.1f, 100.0f);

	// init camera
	c = Camera();

	// load xml project file
	//GPSYSTEMS.loadProject("projects/Tutorial_1/Tutorial_1.xml");
	//GPSYSTEMS.loadProject("projects/Tutorial_2/Tutorial_2.xml");
	//GPSYSTEMS.loadProject("projects/Tutorial_3/Tutorial_3.xml");
	//GPSYSTEMS.loadProject("projects/Tutorial_4/Tutorial_4.xml");
	//GPSYSTEMS.loadProject("projects/gun/_gun.xml");
	//GPSYSTEMS.loadProject("projects/virusPuddles/_virusPuddles.xml");
	GPSYSTEMS.loadProject("projects/customizationTests/customizationTests.xml");
	//GPSYSTEMS.loadProject("projects/shuttle/_shuttle.xml");
	//GPSYSTEMS.loadProject("projects/boids/boids.xml");

	// stage stubs can do anything on the CPU the user finds useful in between
	// stage calls (e.g., changing uniforms, switching between shaders, ...)
	setStageStubs();

	Uint32 totalMS = 0;
	Uint32 iterations = 0;
	// process input events and particle systems
	while (processEvents())
	{
		Uint32 start = SDL_GetTicks();
		GPSYSTEMS.processParticles(glm::mat4(), c.getViewMatrix(), projection);
		totalMS += SDL_GetTicks() - start;
		iterations++;
		if (totalMS > 1000)
		{
			SDL_SetWindowTitle(window.window, std::to_string(iterations * 1000.0f / (float)totalMS).c_str());
			totalMS = 0;
			iterations = 0;
		}

		window.swapWindow();
	}


	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool processEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
			return false;
	}

	// process keys being held
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if (keystate[SDL_SCANCODE_A])
	{
		c.processKeyboard(Camera_Movement::LEFT);
	}
	if (keystate[SDL_SCANCODE_D])
	{
		c.processKeyboard(Camera_Movement::RIGHT);
	}
	if (keystate[SDL_SCANCODE_W])
	{
		c.processKeyboard(Camera_Movement::FORWARD);
	}
	if (keystate[SDL_SCANCODE_S])
	{
		c.processKeyboard(Camera_Movement::BACKWARD);
	}
	if (keystate[SDL_SCANCODE_SPACE])
	{
		GPDATA.setUniformValue("spaceHold", 1);
	}
	else
	{
		GPDATA.setUniformValue("spaceHold", 0);
	}
	if (keystate[SDL_SCANCODE_R])
	{
		GPDATA.setUniformValue("rHold", 1);
	}
	else
	{
		GPDATA.setUniformValue("rHold", 0);
	}



	// process mouse input and position
	c.lastMouseX = c.mouseX;
	c.lastMouseY = c.mouseY;
	// set new camera target only if left button is pressed
	if (SDL_GetMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		c.processMouseMovement();
	}

	glm::vec4 gunPoint = c.getPosition() + c.getUp() * -2.0f + c.getFront() * 5.0f;
	GPDATA.setUniformValue("gunPoint", gunPoint);
	GPDATA.setUniformValue("camPos", c.getPosition());
	GPDATA.setUniformValue("camDir", c.getFront());
	GPDATA.setUniformValue("camUp", c.getUp());
	GPDATA.setUniformValue("mouseXY", glm::vec2(c.mouseX, c.mouseY));

	if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
	{
		GPDATA.setUniformValue("rightHold", 1);
	}
	else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_RIGHT)
	{
		GPDATA.setUniformValue("rightHold", 0);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void setStageStubs()
{
	AbstractStage *stg = nullptr;

	// incrementing animationAngle 
	auto aaInc = [](AbstractStage *as)
	{
		GP_Uniform u;
		GPDATA.getUniform("animationAngle", u);
		GPDATA.setUniformValue("animationAngle", u.value[0].x + 1);
	};

	GPSYSTEMS.getPSystemStage("virus", stg, { "emission" });
	if (stg) stg->setStub(true, std::function<void(AbstractStage *)>(aaInc));

	GPSYSTEMS.getPSystemStage("stage1Prop", stg, { "emission" });
	if (stg) stg->setStub(false, std::function<void(AbstractStage *)>(aaInc));




	// shuttle demo "Brain"
	auto incrementState = [&](AbstractStage *as)
	{
		GP_Uniform newRequest, canStateChange, spaceHold, state;
		
		GPDATA.getUniform("spaceHold", spaceHold);
		if (spaceHold.value[0].x == 0)
		{
			GPDATA.setUniformValue("newStateRequest", 1);
			return;
		}

		GPDATA.getUniform("newStateRequest", newRequest);
		GPDATA.getUniform("canRequestChange", canStateChange);
		if (newRequest.value[0].x == 1 && canStateChange.value[0].x == 1)
		{
			GPDATA.setUniformValue("newStateRequest", 0);

			GPDATA.getUniform("state", state);
			float newState = state.value[0].x + 0.5;

			GPDATA.setUniformValue("state", newState);



			auto transitionToStage = [](GLuint shuttleStage, AbstractStage *as) {
				AbstractStage *stg;

				// activate current psystem update stage
				GPSYSTEMS.getPSystemStage("stage" + std::to_string(GLuint(shuttleStage)) + "Prop", stg, { "update", "paused" });
				stg->removeTag("paused");

				// pass along this stub to the next psystem emission stage
				GPSYSTEMS.getPSystemStage("stage" + std::to_string(GLuint(shuttleStage + 1)) + "Prop", stg, { "emission" });
				if (stg)
				{
					stg->setStub(true, as->startStub);
				}
			};


			// unpause this psystem stage
			if (newState == 0.5)
			{
				auto dec = [](AbstractStage *as)
				{
					GP_Uniform u;
					GPDATA.getUniform("trail1Lifetime", u);

					// if the counter reaches 0 remove this stub from the stage (it has done its job)
					if (u.value[0].x <= 0)
					{
						GPDATA.setUniformValue("canRequestChange", 1);
						as->setStub(true, std::function<void(AbstractStage *)>(nullptr));
					}

					// gradually decrease trail particles lifetime
					// they'll fade sooner due to their shader behaviour
					GPDATA.setUniformValue("trail1Lifetime", u.value[0].x - 0.01);

					// also decrease trail particles emission rate
					GPDATA.getUniform("stage1Trail_toCreate", u);
					GPDATA.setUniformValue("stage1Trail_toCreate", u.value[0].x - 3);
				};

				// add this stub to the trail emission stage
				AbstractStage *stg;
				GPSYSTEMS.getPSystemStage("stage1Trail", stg, { "emission" });
				if (stg)
				{
					GPDATA.setUniformValue("canRequestChange", 0);
					stg->setStub(true, std::function<void(AbstractStage *)>(dec));
				}
			}
			// UGHH, don't know how not to repeat myself so much here
			else if (newState == 1.5)
			{
				auto dec = [](AbstractStage *as)
				{
					GP_Uniform u;
					GPDATA.getUniform("trail2Lifetime", u);

					// if the counter reaches 0 remove this stub from the stage (it has done its job)
					if (u.value[0].x <= 0)
					{
						GPDATA.setUniformValue("canRequestChange", 1);
						as->setStub(true, std::function<void(AbstractStage *)>(nullptr));
					}

					// gradually decrease trail particles lifetime
					// they'll fade sooner due to their shader behaviour
					GPDATA.setUniformValue("trail2Lifetime", u.value[0].x - 0.01);

					// also decrease trail particles emission rate
					GPDATA.getUniform("stage2Trail_toCreate", u);
					GPDATA.setUniformValue("stage2Trail_toCreate", u.value[0].x - 0.6);
				};

				// add this stub to the trail emission stage
				AbstractStage *stg;
				GPSYSTEMS.getPSystemStage("stage2Trail", stg, { "emission" });
				if (stg)
				{
					GPDATA.setUniformValue("canRequestChange", 0);
					stg->setStub(true, std::function<void(AbstractStage *)>(dec));
				}
			}
			else if (newState == 1 || newState == 2)
			{
				transitionToStage(newState, as);
			}
		}
	};

	if (stg) stg->setStub(true, std::function<void(AbstractStage *)>(incrementState));
}