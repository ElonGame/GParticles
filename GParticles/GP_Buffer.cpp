#include "GP_Buffer.h"



GP_Buffer::~GP_Buffer()
{
}

void GP_Buffer::init(GLuint maxParticles)
{
	if (maxParticles > 0)
	{
		elements = maxParticles;
	}

	int multiplier = 1;
	if (type == "vec4")
	{
		multiplier = 4;
	}
	else if (type == "vec2")
	{
		multiplier = 2;
	}

	int bSize = elements * multiplier;


	glGenBuffers(1, &id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(	GL_SHADER_STORAGE_BUFFER, bSize * sizeof(GLfloat), NULL,
					GL_DYNAMIC_DRAW);

	// fill buffer with default values
	GLfloat *values = (GLfloat*)glMapBufferRange(
		GL_SHADER_STORAGE_BUFFER, 0, bSize * sizeof(GLfloat),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < bSize; values[i++] = 0);

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void GP_Buffer::bind(GLuint bindingPoint)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, id);
}
