#include "RendererProgram.h"


RendererProgram::RendererProgram()
{
}


RendererProgram::~RendererProgram()
{
}

void RendererProgram::execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
{
	AbstractProgram::execute(model, view, projection);

	bindResources();

	// render
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPointSize(3.0f);

	if (renderType != "model")
	{
		glAlphaFunc(GL_GREATER, 0);
		glEnable(GL_ALPHA_TEST);

		glEnable(GL_BLEND);

		glDrawArrays(GL_POINTS, 0, maxParticles);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);

		glDrawElementsInstanced(GL_TRIANGLES, geoModel.meshes[0].vertices.size(),
			GL_UNSIGNED_INT, 0, maxParticles);
	}

	AbstractProgram::resetMarkedAtomics();

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::printContents()
{
	std::cout << std::string(80, '-') << std::endl;
	std::cout << "Program Type: Render" << std::endl;

	AbstractProgram::printContents();

	std::cout << "VAO: " << vao << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::bindResources()
{
	AbstractProgram::bindResources();

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindVertexArray(vao);

	glBindTexture(GL_TEXTURE_2D, texture);
}