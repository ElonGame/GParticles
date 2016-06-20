#include "Shader.h"

std::map<std::string, GLenum> Shader::shaderTypeMap =
{
	{ "vertex", GL_VERTEX_SHADER },
	{ "geometry", GL_GEOMETRY_SHADER },
	{ "fragment", GL_FRAGMENT_SHADER },
	{ "compute", GL_COMPUTE_SHADER }
};


Shader::Shader(const std::string &type)
{
	shaderType = type;
	id = glCreateShader(shaderTypeMap.at(shaderType));
}

Shader::Shader(const std::string &type, const std::string source)
{
	shaderType = type;
	GLenum shaderTypeInt = shaderTypeMap.at(shaderType);
	id = glCreateShader(shaderTypeInt);
	loadShader(source);
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
	std::string name = "dumps/" + std::to_string(id) + "_shader.comp";
	std::ofstream out(name);
	out << source;
	out.close();
}

GLuint Shader::getId()
{
	return id;
}

std::string Shader::getShaderType()
{
	return shaderType;
}

std::string Shader::getSource()
{
	return source;
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
