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

private:
	static ParticleSystem loadParticleSystem(TiXmlElement* psystemElement);

	// global resource handles
	static bufferUmap globalBufferInfo;
	static atomicUmap globalAtomicInfo;
	static uniformUmap globalUniformInfo;

	// global resource loading functions
	static bool loadGlobalBuffers(TiXmlElement* buffers);
	static bool loadGlobalAtomics(TiXmlElement* atomics);
	static bool loadGlobalUniforms(TiXmlElement* uniforms);
	static void addGlobalProgramResources(atomicUmap &aum, bufferUmap &bum, uniformUmap &uum);
	static void addAtomicResets(TiXmlElement* actionsElement, atomicUmap &aum, std::vector<GLuint> &aToReset);



	// reserved resource names
	static std::vector<bufferInfo> reservedBufferInfo;
	static std::vector<atomicInfo> reservedAtomicInfo;
	static std::vector<uniformInfo> reservedUniformInfo;

	// reserved resource loading functions
	static bool collectReservedResourceInfo();
	static bool loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemElement);
	static void loadReservedProgramResources(reservedResources &rr, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset);



	// shader program loading functions
	static bool loadComputeProgram(reservedResources &rr, TiXmlElement* programElement, ComputeProgram &cp);
	static bool loadRenderer(reservedResources &rr, TiXmlElement* programElement, RendererProgram &rp);

	// project loading utility functions
	static bool compileShaderFiles(	GLuint shaderID, std::string header, std::string reservedFunctions,
									std::vector<std::string> filePaths, bool dumpToFile = false);
	static std::string generateComputeHeader(atomicUmap &cpAtomicHandles, bufferUmap &cpBufferHandles, uniformUmap &cpUniforms);
	static std::string fileToString(std::string filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint program);
};

