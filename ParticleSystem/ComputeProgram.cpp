#include "ComputeProgram.h"



ComputeProgram::ComputeProgram()
{
}


ComputeProgram::~ComputeProgram()
{
}

void ComputeProgram::execute()
{
	// TODO: check if it can be placed after UseProgram
	bindResources();

	glUseProgram(programhandle);

	// useUniforms
	setUniforms();

	// TODO: change hardcoded work groups
	glDispatchComputeGroupSizeARB((float)512 / 4, 1, 1,
		4, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// see alive particles
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, atomicHandles[0]);
	//GLuint *ptr = (GLuint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//GLuint currentVal = ptr[0];
	//printf("%d ALIVE\n", currentVal);

	// reset marked atomics
	GLuint val = 0;
	for each (auto id in atomicsToReset)
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &val);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	}
}

void ComputeProgram::printContents()
{
	std::cout << ">> Compute program " << programhandle << std::endl;
	std::cout << "Atomics" << std::endl;
	for each (auto b in atomicHandles)
	{
		std::cout << b.first << " with binding " << b.second << std::endl;
	}
	std::cout << "Atomics to Reset" << std::endl;
	for each (auto b in atomicsToReset)
	{
		std::cout << b << std::endl;
	}
	std::cout << "Buffers" << std::endl;
	for each (auto b in bufferHandles)
	{
		std::cout << b.first << " with binding " << b.second << std::endl;
	}
	std::cout << "Uniforms" << std::endl;
	for each (auto b in uniforms)
	{
		std::cout << b.first << " " << b.second.type << " " << b.second.value << " " << std::endl;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::bindResources()
{
	// bind atomics
	for each (auto idBind in atomicHandles)
	{
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, idBind.second, idBind.first);
	}

	// bind buffers
	for each (auto idBind in bufferHandles)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, idBind.second, idBind.first);
	}
}

void ComputeProgram::setUniforms()
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
