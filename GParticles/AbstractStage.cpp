#include "AbstractStage.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractStage::execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
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
void AbstractStage::printContents()
{
	std::cout << "Handle: " << pHandle << std::endl;
	std::cout << "Tags: ";
	for (auto tag : tags)
	{
		std::cout << tag << ", ";
	}
	std::cout << std::endl;


	std::cout << "-- Atomic Buffers --" << std::endl;
	for (auto ab : atomicBuffers)
	{
		std::cout << ab.first << " " << ab.second << std::endl;
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


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractStage::setStub(bool setStartStub, std::function<void(AbstractStage *)> s)
{
	if (setStartStub)
	{
		startStub = s;
	}
	else
	{
		endStub = s;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractStage::resetMarkedAtomics()
{
	GP_AtomicBuffer ab;
	for (auto abHeader : atomicBuffers)
	{
		GPDATA.getAtomicBuffer(abHeader.first, ab);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ab.id);

		for (auto a : ab.atomics)
		{
			if (a.second.reset)
			{
				glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, a.second.offset,
					sizeof(GLuint), &a.second.resetValue);
			}
		}

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AbstractStage::hasTag(std::string tag)
{
	for (auto t : tags)
	{
		if (t == tag)
		{
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractStage::addTag(std::string tag)
{
	tags.insert(tag);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractStage::removeTag(std::string tag)
{
	tags.erase(tag);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AbstractStage::bindResources()
{
	// bind atomics
	for (auto abHeader : atomicBuffers)
	{
		GP_AtomicBuffer ab;
		if (GPDATA.getAtomicBuffer(abHeader.first, ab))
		{
			ab.bind(abHeader.second);
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