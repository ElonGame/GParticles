#pragma once
#include "Utils.h"
#include <map>
#include <fstream>
#include <string>



class Shader
{
public:
	Shader(const std::string &type);
	Shader(const std::string &type, const std::string source);
	~Shader();

	bool loadShader(const std::string source);
	void dumpToFile();

	GLuint getId();
	std::string getShaderType();
	std::string getSource();

private:
	GLuint id;
	std::string shaderType;
	std::string source;
	
	static std::map<std::string, GLenum> shaderTypeMap;

	void Shader::printShaderLog();
};

