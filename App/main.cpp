#include "Utils.h"
#include "Window.h"
#include "Camera.h"
#include <math.h>
#pragma comment(lib,"devil.lib")
#include "GP_Systems.h"

Camera c;
Window window;

bool mouseDown;

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
		GPDATA.setUniformValue("reload", 1);
	}
	else
	{
		GPDATA.setUniformValue("reload", 0);
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

int main(int argc, char* args[])
{
	//if (argc < 2)
	//{
	//	Utils::exitMessage("Invalid Input", "Please provide a project filePath");
	//}

	// init SDL
	window = Window();
	if (!window.init(1024, 576))
	{
		printf("Failed to initialize SDLWindow!\n");
		return 1;
	}

	// init opengl, devIL and set glViewPort
	Utils::init();

	GPDATA.setWindowDimensions(1024, 576);

	// init camera
	c = Camera();


	// load xml project file
	//GPARTICLES.loadProject("projects/Tutorial_1/Tutorial_1.xml");
	//GPARTICLES.loadProject("projects/Tutorial_2/Tutorial_2.xml");
	//GPARTICLES.loadProject("projects/Tutorial_3/Tutorial_3.xml");
	//GPARTICLES.loadProject("projects/Tutorial_4/Tutorial_4.xml");
	GPARTICLES.loadProject("projects/gun/gun.xml");
	//GPARTICLES.loadProject("projects/virusPuddles/_virusPuddles.xml");
	//GPARTICLES.loadProject("projects/boids/boids.xml");


	GP_Uniform u;
	GPDATA.getUniform("animationAngle", u);
	float i = u.value[0].x;
	// system loop
	while (processEvents())
	{
		i++;
		GPDATA.setUniformValue("animationAngle", i);

		float windowRatio = GPDATA.getWindowWidth() / GPDATA.getWindowHeight();
		glm::mat4 projection = glm::perspective(45.0f, windowRatio, 0.1f, 100.0f);

		// process all particle system
		GPARTICLES.processParticles(glm::mat4(), c.getViewMatrix(), projection);

		window.swapWindow();
	}


	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}

