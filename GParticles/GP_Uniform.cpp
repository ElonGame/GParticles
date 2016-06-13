#include "GP_Uniform.h"



GP_Uniform::GP_Uniform()
{
	// default to all 0's instead of identity matrix
	value = glm::mat4(0);
}


GP_Uniform::~GP_Uniform()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Uniform::bind(GLint location)
{
	if (type == "uint")
		glUniform1uiv(location, 1, (GLuint *)&value[0].x);
	else if (type == "float")
		glUniform1fv(location, 1, &value[0].x);
	else if (type == "vec2")
		glUniform2fv(location, 1, &value[0].x);
	else if (type == "vec4")
		glUniform4fv(location, 1, &value[0].x);
	else if (type == "mat4")
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}