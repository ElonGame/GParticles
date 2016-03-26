#include "RendererProgram.h"


RendererProgram::RendererProgram()
{
}


RendererProgram::~RendererProgram()
{
}

void RendererProgram::execute(glm::mat4 &model, Camera cam)
{

	glUseProgram(programhandle);

	bindResources();

	// useUniforms
	setUniforms();

	// TODO: remove and make camera class
	//glm::mat4 model = glm::rotate(model, -35.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 view;
	glm::mat4 projection;

	//model = glm::rotate(model, -35.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
	view = cam.getViewMatrix();
	projection = glm::perspective(45.0f, (GLfloat)1024 / (GLfloat)576, 0.1f, 100.0f);

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(programhandle, "model");
	GLint viewLoc = glGetUniformLocation(programhandle, "view");
	GLint projLoc = glGetUniformLocation(programhandle, "projection");
	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


	// render
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 0, 512);

	// unbind resources
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(NULL);
}

void RendererProgram::printContents()
{
	std::cout << ">> Renderer Program" << std::endl;
	std::cout << "Atomics" << std::endl;
	for each (auto b in atomicHandles)
	{
		std::cout << b << std::endl;
	}
	for each (auto b in uniforms)
	{
		std::cout << b.first << " " << b.second.type << " " << b.second.value << " " << std::endl;
	}
	std::cout << "vao " << vao << std::endl;

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::bindResources()
{
	glBindVertexArray(vao);
	glEnableClientState(GL_VERTEX_ARRAY);

	int bindingPoint = 0;

	// bind atomics
	for each (GLuint id in atomicHandles)
	{
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, bindingPoint++, id);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::setUniforms()
{
	for each (auto u in uniforms)
	{
		int uLocation = glGetUniformLocation(programhandle, u.first.c_str());

		if (u.second.type == "float")
			glUniform1f(uLocation, u.second.value);
		else
			; // TODO: support vec4
			  // TODO: support mat4
	}
}
