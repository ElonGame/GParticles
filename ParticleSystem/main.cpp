#include "Utils.h"
#include "Window.h"
#include "ParticleSystemLoader.h"
#include "ParticleSystemManager.h"
#include "Camera.h"
#include <IL\il.h>
#pragma comment(lib,"devil.lib")
#include "Model.h"

Camera c;
std::vector<Texture> g_textures;


void fetchParticleSystems(ParticleSystemManager &psm)
{
	// TODO: unbind current state and delete everything

	std::vector<ParticleSystem> psContainer;
	ParticleSystemLoader::loadProject("shaders/fireworks/userInput.xml", psContainer);

	psm.setContainer(psContainer);
}


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
	int mouseX, mouseY;
	// set new camera target only if left button is pressed
	if (SDL_GetMouseState(&c.mouseX, &c.mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		c.processMouseMovement();
	}
	
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

	// init opengl stuff
	Utils::initGL();

	ilInit();

	// init camera
	c = Camera();

	// init ParticleSystemManager and load particle systems
	ParticleSystemManager psManager = ParticleSystemManager();
	fetchParticleSystems(psManager);






	Model myModel = Model();
	//myModel = Model("assets/models/nanosuit2/nanosuit.obj");
	myModel = Model("assets/models/agent/agent.blend");
	//myModel = Model("assets/models/paperPlane/3d-model.obj");
	//Model myModel("assets/models/sphere/sphere.obj");

	GLuint modelProgram = glCreateProgram();

	// vertex shader
	GLuint rpVertShader = glCreateShader(GL_VERTEX_SHADER);
	std::vector<std::string> paths1;
	paths1.push_back(std::string("shaders/model.vert"));
	ParticleSystemLoader::compileShaderFiles(rpVertShader, paths1);
	glAttachShader(modelProgram, rpVertShader);

	// frag shader
	GLuint rpFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::vector<std::string> paths2;
	paths2.push_back(std::string("shaders/model.frag"));
	ParticleSystemLoader::compileShaderFiles(rpFragShader, paths2);
	glAttachShader(modelProgram, rpFragShader);

	glLinkProgram(modelProgram);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(modelProgram, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", modelProgram);
		return false;
	}

	printf("MODEL PROGRAM NUMBER IS %d!\n", modelProgram);




	// system loop
	while (processEvents())
	{
		// TODO: system ticks here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);

		// process all particle system
		psManager.processParticles(c);
		glDisable(GL_BLEND);

		glUseProgram(modelProgram);

		glm::mat4 projection = glm::perspective(45.0f, (GLfloat)1024 / (GLfloat)576, 0.1f, 100.0f);
		glm::mat4 view = c.getViewMatrix();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0,-0.2f,-3.0f));
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(modelProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		myModel.draw();

		glUseProgram(NULL);
		glDisable(GL_DEPTH_TEST);




		window.swapWindow();
	}

	// TODO: deallocate programs
	// close window and deallocate SDL
	window.quit();

	return 0;
}