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


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Uniform::setValue(float val)
{
	if (type == "uint" && val >= 0 || type == "float")
	{
		value[0].x = val;

		return true;
	}
	
	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Uniform::setValue(glm::vec2 val)
{
	if (type == "vec2")
	{
		value[0].x = val.x;
		value[0].y = val.y;

		return true;
	}
	
	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Uniform::setValue(glm::vec4 val)
{
	if (type == "vec4")
	{
		value[0] = val;

		return true;
	}
	
	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Uniform::setValue(glm::mat4 val)
{
	if (type == "mat4")
	{
		value = val;

		return true;
	}
	
	return false;
}