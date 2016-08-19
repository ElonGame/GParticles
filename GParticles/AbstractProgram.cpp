#include "AbstractProgram.h"

AbstractProgram::AbstractProgram()
{
}

AbstractProgram::~AbstractProgram()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractProgram::execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
{
	// initialize lastStep on program's first execution
	if (firstExec)
	{
		lastStep = GPDATA.getCurrentTimeMillis();
		firstExec = false;
	}

	GLuint timeSpan = GPDATA.getCurrentTimeMillis() - lastStep;

	if (timeSpan < iterationStep)
	{
		canKeepUpIteration = true;
		return;
	}

	// set elapsed time since last iteration for later binding
	GPDATA.setUniformValue(psystem + "_deltaTime", timeSpan / 1000.0f);

	if (canKeepUpIteration)
	{	// add iterationStep to lastStep as to reduce error accumulation
		lastStep += iterationStep;
		canKeepUpIteration = false;
	}
	else
	{	// add timeSpan to lastStep so a "spiral of death" is not created
		// where the program falls increasingly behind with each iteration
		lastStep += timeSpan;
	}

	glUseProgram(pHandle);

	bindResources();

	// get matrices uniform location
	GLint modelLoc = glGetUniformLocation(pHandle, "model");
	GLint viewLoc = glGetUniformLocation(pHandle, "view");
	GLint projLoc = glGetUniformLocation(pHandle, "projection");

	// pass matrices to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractProgram::printContents()
{
	std::cout << "Handle: " << pHandle << std::endl;
	std::cout << "Tags: ";
	for (auto tag : tags)
	{
		std::cout << tag << ", ";
	}
	std::cout << std::endl;


	std::cout << "-- Atomics --" << std::endl;
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GPDATA.getAtomic(aName.first, a))
		{
			std::cout << a.name << " with id " << a.id << " and resetValue "
				<< a.resetValue << std::endl;
		}
	}

	std::cout << "-- Uniforms --" << std::endl;
	for (auto uName : uniforms)
	{
		GP_Uniform u;
		if (GPDATA.getUniform(uName, u))
		{
			std::cout << u.name << " " << u.type << " " << u.value[0].x
				<< " " << std::endl;
		}
	}
}

void AbstractProgram::resetMarkedAtomics()
{
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GPDATA.getAtomic(aName.first, a) && a.reset == true)
		{
			a.setCurrentValue(a.resetValue);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractProgram::bindResources()
{
	// bind atomics
	for (auto aName : atomics)
	{
		GP_Atomic a;
		if (GPDATA.getAtomic(aName.first, a))
		{
			a.bind(aName.second);
		}
	}

	// bind uniforms
	for (auto uName : uniforms)
	{
		GP_Uniform u;
		if (GPDATA.getUniform(uName, u))
		{
			u.bind(glGetUniformLocation(pHandle, uName.c_str()));
		}
	}
}