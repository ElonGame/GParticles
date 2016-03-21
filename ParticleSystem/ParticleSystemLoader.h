#pragma once
#include <unordered_map>
#include <fstream>
#include "ParticleSystem.h"
#include "ComputeProgram.h"
#include "RendererProgram.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"

struct bufferInfo
{
	GLuint id;
	GLuint elements;
	std::string type;
};

struct atomicInfo
{
	GLuint id;
	GLuint initialValue;
};

//struct uniformInfo
//{
//	GLfloat value;
//	std::string type;
//};

using bufferUmap = std::unordered_map<std::string, bufferInfo>;
using atomicUmap = std::unordered_map<std::string, atomicInfo>;
using uniformUmap = std::unordered_map<std::string, uniformInfo>;

class ParticleSystemLoader
{
public:
	ParticleSystemLoader();
	~ParticleSystemLoader();

	ParticleSystem loadParticleSystem(std::string filePath);

private:
	bufferUmap bufferHandles;
	atomicUmap atomicHandles;
	uniformUmap uniformHandles;

	bool initBuffers(TiXmlElement* buffers);
	bool initAtomics(TiXmlElement* atomics);
	bool initUniforms(TiXmlElement* uniforms);

	bool loadRenderer(TiXmlElement* glpElement, RendererProgram &rp);
	bool loadGLProgram(TiXmlElement* glpElement, ComputeProgram &glp);

	bool compileShaderFiles(GLuint shaderID, std::string header, std::string reservedFunctions, std::vector<std::string> filePaths, bool dumpToFile = false);
	std::string generateHeader(atomicUmap glpAtomicHandles, bufferUmap glpBufferHandles, uniformUmap glpUniforms);
	std::string fileToString(std::string filePath);
	void printShaderLog(GLuint shader);
	void printProgramLog(GLuint program);
};

