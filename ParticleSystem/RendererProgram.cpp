#include "RendererProgram.h"



RendererProgram::RendererProgram()
{
}


RendererProgram::~RendererProgram()
{
}

void RendererProgram::execute()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(programhandle);

	bindResources();

	// useUniforms
	setUniforms();

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
