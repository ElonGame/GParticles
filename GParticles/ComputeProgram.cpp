#include "ComputeProgram.h"

ComputeProgram::ComputeProgram()
{
}

ComputeProgram::~ComputeProgram()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
{
	AbstractProgram::execute(model, view, projection);

	bindResources();

	// calculate group size rounded up and process data
	GLuint groupSize = (maxParticles + numWorkGroups - 1) / numWorkGroups;
	glDispatchComputeGroupSizeARB(groupSize, 1, 1, numWorkGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	AbstractProgram::resetMarkedAtomics();

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::printContents()
{
	std::cout << std::string(80, '-') << std::endl;
	std::cout << "Type: Compute" << std::endl;

	AbstractProgram::printContents();

	std::cout << "-- Buffers --" << std::endl;
	for (auto bName : buffers)
	{
		GP_Buffer b;
		if (GPDATA.getBuffer(bName.first, b))
		{
			std::cout << b.name << " with binding " << b.id << std::endl;
		}
	}

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComputeProgram::bindResources()
{
	AbstractProgram::bindResources();

	// bind buffers
	for (auto bName : buffers)
	{
		GP_Buffer b;
		if (GPDATA.getBuffer(bName.first, b))
		{
			b.bind(bName.second);
		}
	}
}