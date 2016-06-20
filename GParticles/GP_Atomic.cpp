#include "GP_Atomic.h"





GP_Atomic::~GP_Atomic()
{
}

void GP_Atomic::init()
{
	glGenBuffers(1, &id);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &resetValue);
}

void GP_Atomic::bind(GLuint bindingPoint)
{
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, id);
}

GLuint GP_Atomic::getCurrentValue()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	GLuint *ptr = (GLuint *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	return ptr[0];
}

void GP_Atomic::setCurrentValue(GLuint value)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &value);
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
}
