#pragma once
#include <unordered_map>
#include <fstream>
#include "ParticleSystem.h"
#include "ComputeProgram.h"
#include "RendererProgram.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"


using bufferUmap = std::unordered_map<std::string, bufferInfo>;
using atomicUmap = std::unordered_map<std::string, atomicInfo>;
using uniformUmap = std::unordered_map<std::string, uniformInfo>;


class ParticleSystemLoader
{
public:
	// parses file and fills psContainer with ParticleSystem objects
	static bool loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer);
	static ParticleSystem loadParticleSystem(TiXmlElement* psystemElement);

private:
	// global resources
	static bufferUmap bufferHandles;
	static atomicUmap atomicHandles;
	static uniformUmap uniformHandles;

	// global resource loading functions
	static bool loadBuffers(TiXmlElement* buffers);
	static bool loadAtomics(TiXmlElement* atomics);
	static bool loadUniforms(TiXmlElement* uniforms);
	static void loadProgramResources(TiXmlElement* resources, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset);

	// shader program loading functions
	static bool loadComputeProgram(TiXmlElement* glpElement, ComputeProgram &glp);
	static bool loadRenderer(TiXmlElement* glpElement, RendererProgram &rp);

	// project loading utility functions
	static bool compileShaderFiles(	GLuint shaderID, std::string header, std::string reservedFunctions,
									std::vector<std::string> filePaths, bool dumpToFile = false);
	static std::string generateHeader(atomicUmap glpAtomicHandles, bufferUmap glpBufferHandles, uniformUmap glpUniforms);
	static std::string fileToString(std::string filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint program);
};

