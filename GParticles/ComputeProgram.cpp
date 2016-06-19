#include "ComputeProgram.h"


ComputeProgram::ComputeProgram()
{
}


ComputeProgram::~ComputeProgram()
{
}

void ComputeProgram::execute(GLuint numWorkGroups)
{
	auto timeSpan = timeClock::now() - lastStep;

	if (timeSpan < iterationStep)
	{
		return;
	}

	lastStep += iterationStep;

	glUseProgram(programhandle);

	bindResources();

	glDispatchComputeGroupSizeARB((float)maxParticles / numWorkGroups, 1, 1,
		numWorkGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// reset marked atomics
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GlobalData::get().getAtomic(aName, a))
		{
			a.setCurrentValue(a.resetValue);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::printContents()
{
	std::cout << ">> Compute program " << programhandle << std::endl;
	std::cout << "Atomics" << std::endl;
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GlobalData::get().getAtomic(aName, a))
		{
			std::cout << a.name << " with id " << a.id << " and resetValue " << a.resetValue << std::endl;
		}
	}

	std::cout << "Buffers" << std::endl;
	for (auto b : buffers)
	{
		std::cout << b.first << " with binding " << b.second.id << std::endl;
	}

	std::cout << "Uniforms" << std::endl;
	for (auto uName : uniforms)
	{
		GP_Uniform u;
		if (GlobalData::get().getUniform(uName, u))
		{
			std::cout << u.name << " " << u.type << " " << u.value[0].x << " " << std::endl;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::bindResources()
{
	// bind buffers
	for (auto b : buffers)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, b.second.binding, b.second.id);
	}

	// bind atomics
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GlobalData::get().getAtomic(aName, a))
		{
			a.bind();
		}
	}

	// bind uniforms
	for (auto uName : uniforms)
	{
		GP_Uniform u;
		if (GlobalData::get().getUniform(uName, u))
		{
			u.bind(glGetUniformLocation(programhandle, uName.c_str()));
		}
	}
}