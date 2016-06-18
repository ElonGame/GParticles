#include "Utils.h"
#include "Window.h"
#include "GPLoader.h"
#include "GPManager.h"
#include "Camera.h"
#include <IL\il.h>
#include <math.h>
#pragma comment(lib,"devil.lib")
#include "Model.h"

Camera c;
std::vector<Texture> g_textures;

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

	// process mouse input and position
	c.lastMouseX = c.mouseX;
	c.lastMouseY = c.mouseY;
	// set new camera target only if left button is pressed
	if (SDL_GetMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		c.processMouseMovement();
	}

	/*GP_Uniform u;
	GlobalData::get().getUniform("mouseXY", u);
	u.value[0] = glm::vec4(	c.mouseX / GlobalData::get().getWindowWidth(),
							c.mouseY / GlobalData::get().getWindowHeight(),
							0, 0);
	GlobalData::get().addUniform(u);*/

	//GlobalData::get().setUniform("mouseXY", -1 / GlobalData::get().getWindowWidth());

	//glm::vec2 asd = glm::vec2(0.1f, 0);

	//GlobalData::get().setUniformValue("mouseXY", asd);
	//GlobalData::get().setUniformValue("uniTest", 13);


	return true;
}



int main(int argc, char* args[])
{
	// init SDL
	Window window = Window();
	if (!window.init(1024, 576))
	{
		printf("Failed to initialize SDLWindow!\n");
		return 1;
	}

	GlobalData::get().setWindowDimensions(1024, 576);

	// init opengl stuff
	Utils::initGL();

	ilInit();

	// init camera
	c = Camera();

	// init GPManager and load xml project file
	GPManager gpManager;
	//GPLoader::loadProject("shaders/fireworks/fireworks.xml", gpManager.pSystems);
	GPLoader::loadProject("shaders/rain_and_puddles/_test.xml", gpManager.pSystems);
	//GPLoader::loadProject("shaders/boids/boids.xml", gpManager.pSystems);
	//GPLoader::loadProject("shaders/Tutorial_1/_test.xml", gpManager.pSystems);


	//Model myModel = Model("assets/models/nanosuit2/nanosuit.obj");
	//Model myModel = Model("assets/models/agent/agent.blend");
	//myModel = Model("assets/models/paperPlane/3d-model.obj");
	//Model myModel("assets/models/sphere/sphere.obj");
	//Model myModel = Model("assets/models/virus/virus.3ds");

	//GLuint modelProgram = glCreateProgram();

	//// vertex shader
	//GLuint rpVertShader = glCreateShader(GL_VERTEX_SHADER);
	//std::vector<std::string> paths1;
	//paths1.push_back(std::string("shaders/rain_and_puddles/model.vert"));
	//GPLoader::compileShaderFiles(rpVertShader, paths1);
	//glAttachShader(modelProgram, rpVertShader);

	//// frag shader
	//GLuint rpFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	//std::vector<std::string> paths2;
	//paths2.push_back(std::string("shaders/rain_and_puddles/model.frag"));
	//GPLoader::compileShaderFiles(rpFragShader, paths2);
	//glAttachShader(modelProgram, rpFragShader);

	//glLinkProgram(modelProgram);

	//GLint programSuccess = GL_FALSE;
	//glGetProgramiv(modelProgram, GL_LINK_STATUS, &programSuccess);
	//if (programSuccess == GL_FALSE)
	//{
	//	printf("Error linking program %d!\n", modelProgram);
	//	return false;
	//}

	//printf("MODEL PROGRAM NUMBER IS %d!\n", modelProgram);


	GP_Uniform u;
	GlobalData::get().getUniform("virusAnimationAngle", u);
	float i = u.value[0].x;
	// system loop
	while (processEvents())
	{
		i += 0.01f;
		// user uniform manual control
		/*GP_Uniform time;
		GlobalData::get().getUniform("time", time);
		time.value
		int timeLoc = glGetUniformLocation(programhandle, "timet");
		glUniform1ui(timeLoc, GlobalData::get().getCurrentTimeMillis());*/


		// TODO: system ticks here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		// process all particle system
		gpManager.processParticles(c.getViewMatrix());
		glDisable(GL_BLEND);

		GlobalData::get().setUniformValue("virusAnimationAngle", ++i);

		glUseProgram(NULL);
		glDisable(GL_DEPTH_TEST);




		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}

