#include "RendererProgram.h"


RendererProgram::RendererProgram()
{
}


RendererProgram::~RendererProgram()
{
}

void RendererProgram::execute(glm::mat4 &modelMat, glm::mat4 &viewMat, glm::mat4 &projectionMat)
{
	glUseProgram(programhandle);

	bindResources();

	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(programhandle, "model");
	GLint viewLoc = glGetUniformLocation(programhandle, "view");
	GLint projLoc = glGetUniformLocation(programhandle, "projection");

	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMat));

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

		glDrawElementsInstanced(GL_TRIANGLES, model.meshes[0].vertices.size(),
								GL_UNSIGNED_INT, 0, maxParticles);
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
	std::cout << ">> Renderer Program " << programhandle << std::endl;
	std::cout << "Atomics" << std::endl;
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GP_Data::get().getAtomic(aName.first, a))
		{
			std::cout << a.name << " with id " << a.id << " and resetValue " << a.resetValue << std::endl;
		}
	}

	std::cout << "Uniforms" << std::endl;
	for (auto uName : uniforms)
	{
		GP_Uniform u;
		if (GP_Data::get().getUniform(uName, u))
		{
			std::cout << u.name << " " << u.type << " " << u.value[0].x << " " << std::endl;
		}
	}

	std::cout << "vao " << vao << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RendererProgram::bindResources()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindVertexArray(vao);
	

	// bind atomics
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GP_Data::get().getAtomic(aName.first, a))
		{
			a.bind(aName.second);
		}
	}

	// bind uniforms
	for (auto uName : uniforms)
	{
		GP_Uniform u;
		if (GP_Data::get().getUniform(uName, u))
		{
			u.bind(glGetUniformLocation(programhandle, uName.c_str()));
		}
	}

	glBindTexture(GL_TEXTURE_2D, texture);
}