#pragma once
#include "Utils.h"
#include <map>
#include <fstream>
#include <string>



class Shader
{
public:
	Shader(const std::string &shaderType);
	Shader(const std::string &shaderType, const std::string shaderSource, const std::string shaderName);
	~Shader();

	bool loadShader(const std::string shaderSource);
	void dumpToFile();

	GLuint getId();
	std::string getType();
	std::string getSource();
	std::string getName();

private:
	GLuint id;
	std::string type;
	std::string source;
	std::string name;
	
	static std::map<std::string, GLenum> shaderTypeMap;

	void Shader::printShaderLog();
};

