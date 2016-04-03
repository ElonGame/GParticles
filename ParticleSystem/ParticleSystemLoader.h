#pragma once
#include <fstream>
#include "ParticleSystem.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"

struct reservedResources
{
	GLuint maxParticles;
	bufferUmap reservedBufferHandles;
	atomicUmap reservedAtomicHandles;
	uniformUmap reservedUniformHandles;
};

class ParticleSystemLoader
{
public:
	// parses file and fills psContainer with ParticleSystem objects
	static bool loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer);
	static ParticleSystem loadParticleSystem(TiXmlElement* psystemElement);

private:
	// global resource handles
	static bufferUmap globalBufferHandles;
	static atomicUmap globalAtomicHandles;
	static uniformUmap globalUniformHandles;

	// global resource loading functions
	static bool loadGlobalBuffers(TiXmlElement* buffers);
	static bool loadGlobalAtomics(TiXmlElement* atomics);
	static bool loadGlobalUniforms(TiXmlElement* uniforms);
	static void loadGlobalProgramResources(TiXmlElement* actionsElement, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset);



	// reserved resource names
	static std::vector<bufferInfo> reservedBufferInfo;
	static std::vector<atomicInfo> reservedAtomicInfo;
	static std::vector<uniformInfo> reservedUniformInfo;

	// reserved resource loading functions
	static bool collectReservedResourceInfo();
	static bool loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemElement);
	static void loadReservedProgramResources(reservedResources &rr, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset);



	// shader program loading functions
	static bool loadComputeProgram(reservedResources &rr, TiXmlElement* programElement, ComputeProgram &glp);
	static bool loadRenderer(reservedResources &rr, TiXmlElement* programElement, RendererProgram &rp);

	// project loading utility functions
	static bool compileShaderFiles(	GLuint shaderID, std::string header, std::string reservedFunctions,
									std::vector<std::string> filePaths, bool dumpToFile = false);
	static std::string generateComputeHeader(atomicUmap &glpAtomicHandles, bufferUmap &glpBufferHandles, uniformUmap &glpUniforms);
	static std::string fileToString(std::string filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint program);
};

