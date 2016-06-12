#include "RendererProgram.h"


RendererProgram::RendererProgram()
{
}


RendererProgram::~RendererProgram()
{
}

void RendererProgram::execute(glm::mat4 &modelMat, glm::mat4 &viewMat)
{
	glUseProgram(programhandle);

	bindResources();

	setUniforms();

	GLfloat windowW = (GLfloat)GlobalData::getInstance().getWindowWidth();
	GLfloat windowH = (GLfloat)GlobalData::getInstance().getWindowHeight();
	glm::mat4 projection = glm::perspective(45.0f, windowW / windowH, 0.1f, 100.0f);

	glm::mat4 normalMat = viewMat * modelMat;
	normalMat = glm::transpose(glm::inverse(normalMat));

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(programhandle, "model");
	GLint viewLoc = glGetUniformLocation(programhandle, "view");
	GLint projLoc = glGetUniformLocation(programhandle, "projection");
	GLint normalMatLoc = glGetUniformLocation(programhandle, "normalMatrix");

	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

	// render
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(3.0f);

	if (renderType != "model")
	{
		glDrawArrays(GL_POINTS, 0, 512);
		//glBindBuffer(GL_ARRAY_BUFFER, 8);
		//GLfloat *ptr = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		//glUnmapBuffer(GL_ARRAY_BUFFER);
		//GLfloat currentVal = ptr[2];
		//printf("%d LAST\n", currentVal);
	}
	else
	{

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		
		glDrawElementsInstanced(GL_TRIANGLES, model.meshes[0].vertices.size(), GL_UNSIGNED_INT, 0, 30);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

	}

	// unbind resources
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(NULL);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::printContents()
{
	std::cout << ">> Renderer Program" << std::endl;
	std::cout << "Atomics" << std::endl;
	for each (auto b in atomicHandles)
	{
		std::cout << b.first << " with binding " << b.second.id << std::endl;
	}
	std::cout << "Uniforms" << std::endl;
	for each (auto b in uniforms)
	{
		std::cout << b.first << " " << b.second.type << " " << b.second.value[0].x << " " << std::endl;
	}
	std::cout << "vao " << vao << std::endl;

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::bindResources()
{
	glBindVertexArray(vao);
	glEnableClientState(GL_VERTEX_ARRAY);

	// bind atomics
	for (auto atm : atomicHandles)
	{
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, atm.second.binding, atm.second.id);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::setUniforms()
{
	for (auto u : uniforms)
	{
		u.second.bind(glGetUniformLocation(programhandle, u.first.c_str()));
	}
}
