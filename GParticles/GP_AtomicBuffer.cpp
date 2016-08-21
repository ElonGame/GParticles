#include "GP_AtomicBuffer.h"

GP_AtomicBuffer::~GP_AtomicBuffer()
{
}

void GP_AtomicBuffer::init()
{
	glGenBuffers(1, &id);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * atomics.size(), NULL, GL_DYNAMIC_DRAW);

	// set initial atomics values
	for (auto a : atomics)
	{
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &a.second.resetValue);
	}

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void GP_AtomicBuffer::bind(GLuint bindingPoint)
{
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, id);
}