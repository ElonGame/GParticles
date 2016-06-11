#include "ComputeProgram.h"


ComputeProgram::ComputeProgram()
{
}


ComputeProgram::~ComputeProgram()
{
}

void ComputeProgram::execute(GLuint numWorkGroups)
{
	glUseProgram(programhandle);

	bindResources();

	setUniforms();

	glDispatchComputeGroupSizeARB((float)maxParticles / numWorkGroups, 1, 1,
		numWorkGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//// print random counter
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, atomics.at("randomCounter").id);
	//GLuint *ptr = (GLuint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//GLuint currentVal = ptr[0];
	//if (programhandle == 1)
	//	printf("%d LAST\n", currentVal);

	// reset marked atomics
	for (auto a : atomics)
	{
		if (a.second.reset)
		{
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, a.second.id);
			glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
							&a.second.initialValue);
			glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::printContents()
{
	std::cout << ">> Compute program " << programhandle << std::endl;
	std::cout << "Atomics" << std::endl;
	for (auto b : atomics)
	{
		std::cout << b.first << " with id " << b.second.id << ".";
		if (b.second.reset)
		{
			std::cout << " [OVERRIDEN] to " << b.second.initialValue << " every iteration.";
		}
		
		std::cout << std::endl;
	}

	std::cout << "Buffers" << std::endl;
	for (auto b : buffers)
	{
		std::cout << b.first << " with binding " << b.second.id << std::endl;
	}
	std::cout << "Uniforms" << std::endl;
	for (auto b : uniforms)
	{
		std::cout << b.first << " " << b.second.type << " " << b.second.value << " " << std::endl;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::bindResources()
{
	// bind atomics
	for each (auto a in atomics)
	{
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, a.second.binding, a.second.id);
	}

	// bind buffers
	for each (auto b in buffers)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, b.second.binding, b.second.id);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::setUniforms()
{
	for (auto u : uniforms)
	{
		int uLocation = glGetUniformLocation(programhandle, u.first.c_str());

		if (u.second.type == "float")
			glUniform1f(uLocation, u.second.value);
		else
			; // TODO: support vec4
			  // TODO: support mat4
	}

	int timeLoc = glGetUniformLocation(programhandle, "timet");
	glUniform1ui(timeLoc, GlobalData::getInstance().getCurrentTimeMillis());

	int mouseLoc = glGetUniformLocation(programhandle, "mouse");
	glUniform2f(mouseLoc, GlobalData::getInstance().getMouseX(true), GlobalData::getInstance().getMouseY(true));
}
