#include "Pipeline.h"
struct pos { float x, y, z, w; };
struct vel { float vx, vy, vz, vw; };


Pipeline::Pipeline()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pipeline::initBuffers()
{
	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	// index
	glGenBuffers(1, &indexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numParticles * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	GLfloat *index = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, numParticles * sizeof(GLfloat), bufMask);
	for (int i = 0; i < numParticles; i++)
	{
		index[i] = i;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// lifetime
	glGenBuffers(1, &lifeSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lifeSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numParticles * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	GLfloat *lifetimes = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, numParticles * sizeof(GLfloat), bufMask);
	for (int i = 0; i < numParticles; i++)
	{
		lifetimes[i] = 0.6f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.4f - 0.6f)));
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//position
	glGenBuffers(1, &posSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numParticles * sizeof(struct pos), NULL, GL_DYNAMIC_DRAW);

	struct pos *points = (struct pos*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, numParticles * sizeof(struct pos), bufMask);
	for (int i = 0; i < numParticles; i++)
	{
		points[i].x = 0;
		points[i].y = 0;
		points[i].z = 0;
		points[i].w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//velocity
	glGenBuffers(1, &velSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numParticles * sizeof(struct vel), NULL, GL_DYNAMIC_DRAW);

	struct vel *velocities = (struct vel*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, numParticles * sizeof(struct vel), bufMask);
	for (int i = 0; i < numParticles; i++)
	{
		velocities[i].vx = -0.01f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.01f + 0.01f)));
		velocities[i].vy = -0.01f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.01f + 0.01f)));
		velocities[i].vz = -0.01f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.01f + 0.01f)));
		velocities[i].vw = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// atomic counters
	glGenBuffers(1, &aliveParticles);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, aliveParticles);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	GLuint val = numParticles;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val);

	glGenBuffers(1, &emissionAttempts);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, emissionAttempts);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	val = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val);

	glGenBuffers(1, &created);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, created);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	val = 0;
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pipeline::initEmitterProgram()
{
	emitterHandle = glCreateProgram();

	GLuint emitterShader = glCreateShader(GL_COMPUTE_SHADER);

	compileShaderFiles(emitterShader, "shaders/emitter.comp");

	glAttachShader(emitterHandle, emitterShader);

	glLinkProgram(emitterHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(emitterHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", emitterHandle);
		printProgramLog(emitterHandle);
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pipeline::initUpdaterProgram()
{
	updaterHandle = glCreateProgram();

	GLuint compShader = glCreateShader(GL_COMPUTE_SHADER);

	compileShaderFiles(compShader, "shaders/updater.comp");

	glAttachShader(updaterHandle, compShader);

	glLinkProgram(updaterHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(updaterHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", updaterHandle);
		printProgramLog(updaterHandle);
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pipeline::initRendererProgram()
{
	rendererHandle = glCreateProgram();

	// vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileShaderFiles(vertexShader, "shaders/vertex.vert");
	glAttachShader(rendererHandle, vertexShader);

	// fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileShaderFiles(fragmentShader, "shaders/frag.frag");
	glAttachShader(rendererHandle, fragmentShader);

	glLinkProgram(rendererHandle);

	// generate vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// add vao buffers
	GLint posLocation = glGetAttribLocation(rendererHandle, "position");
	glBindBuffer(GL_ARRAY_BUFFER, posSSBO);
	glEnableVertexAttribArray(posLocation);
	glVertexAttribPointer(posLocation, 4, GL_FLOAT, 0, 0, 0);

	GLint lifeLocation = glGetAttribLocation(rendererHandle, "lifetime");
	glBindBuffer(GL_ARRAY_BUFFER, lifeSSBO);
	glEnableVertexAttribArray(lifeLocation);
	glVertexAttribPointer(lifeLocation, 1, GL_FLOAT, 0, 0, 0);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pipeline::emit()
{
	auto currentTime = timeClock::now();

	if ((currentTime - lastStep) > emissionStep)
	{
		// bind buffers
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, aliveParticles);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, emissionAttempts);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 3, created);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, lifeSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, posSSBO);


		// udpate buffers
		glUseProgram(emitterHandle);

		glUniform1f(glGetUniformLocation(emitterHandle,"initialLife"), 3.0f);
		glUniform1ui(glGetUniformLocation(emitterHandle, "toCreate"), 1);

		glDispatchComputeGroupSizeARB(	(float)numParticles / workGroupSize, 1, 1,
										workGroupSize, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		

		// see emission count
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, emissionAttempts);
		GLuint *ptr = (GLuint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		GLuint  currentVal = ptr[0];
		printf("%d emissionTests\n", currentVal);

		// emission count reset
		GLuint val = 0;
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, emissionAttempts);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

		// see emission count
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, created);
		ptr = (GLuint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		currentVal = ptr[0];
		printf("%d created\n", currentVal);

		// emission count reset
		val = 0;
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, created);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

		lastStep = currentTime;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pipeline::update()
{
	// bind buffers
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, aliveParticles);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, indexSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, lifeSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, posSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, velSSBO);

	// udpate buffers
	glUseProgram(updaterHandle);
	glDispatchComputeGroupSizeARB(	ceil((float)numParticles / workGroupSize), 1, 1,
									workGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pipeline::render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(rendererHandle);
	glBindVertexArray(vao);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, aliveParticles);
	GLuint *ptr = (GLuint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	GLuint currentVal = ptr[0];
	printf("%d ALIVE\n", currentVal);
	
	glUniform1f(glGetUniformLocation(rendererHandle, "alive"), currentVal);

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	model = glm::rotate(model, -35.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f,-2.0f));
	projection = glm::perspective(45.0f, (GLfloat)1024 / (GLfloat)576, 0.1f, 100.0f);

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(rendererHandle, "model");
	GLint viewLoc = glGetUniformLocation(rendererHandle, "view");
	GLint projLoc = glGetUniformLocation(rendererHandle, "projection");
	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 0, numParticles);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(NULL);
}