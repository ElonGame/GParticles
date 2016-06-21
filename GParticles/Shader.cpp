#include "Shader.h"

std::map<std::string, GLenum> Shader::shaderTypeMap =
{
	{ "vertex", GL_VERTEX_SHADER },
	{ "geometry", GL_GEOMETRY_SHADER },
	{ "fragment", GL_FRAGMENT_SHADER },
	{ "compute", GL_COMPUTE_SHADER }
};


Shader::Shader(const std::string &shaderType)
{
	type = shaderType;
	id = glCreateShader(shaderTypeMap.at(type));
}

Shader::Shader(const std::string &shaderType, const std::string shaderSource, const std::string shaderName)
{
	type = shaderType;
	name = shaderName;
	GLenum shaderTypeInt = shaderTypeMap.at(type);
	id = glCreateShader(shaderTypeInt);
	loadShader(shaderSource);
}


Shader::~Shader()
{
	//glDeleteShader(id);
}

bool Shader::loadShader(const std::string sourceString)
{
	source = sourceString;
	const GLchar* shaderSource = source.c_str();
	glShaderSource(id, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(id);

	dumpToFile();

	GLint cShaderCompiled = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &cShaderCompiled);
	if (cShaderCompiled != GL_TRUE)
	{
		printShaderLog();
		return false;
	}

	return true;
}

void Shader::dumpToFile()
{
	std::string finalName = "dumps/" + name + "_" + type + ".glsl";
	std::ofstream out(finalName);
	out << source;
	out.close();
}

GLuint Shader::getId()
{
	return id;
}

std::string Shader::getType()
{
	return type;
}

std::string Shader::getSource()
{
	return source;
}

std::string Shader::getName()
{
	return name;
}

void Shader::printShaderLog()
{
	printf("Unable to compile compute shader %d!\n", id);

	int infoLogLength = 0;
	int maxLength = 0;

	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
	char* infoLog = new char[maxLength];
	
	glGetShaderInfoLog(id, maxLength, &infoLogLength, infoLog);
	if (infoLogLength > 0)
	{
		printf("%s\n", infoLog);
	}

	delete[] infoLog;
}
