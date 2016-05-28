#pragma once
#include <fstream>
#include "ParticleSystem.h"
#include "Texture.h"
#include "Model.h"
#include "Shader.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"

struct reservedResources
{
	GLuint maxParticles;
	bufferUmap reservedBufferInfo;
	atomicUmap reservedAtomicInfo;
	uniformUmap reservedUniformInfo;
};

class GPLoader
{
public:
	// parses file and fills psContainer with ParticleSystem objects
	static bool loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer);
	static bool compileShaderFiles(GLuint shaderID, std::vector<std::string> filePaths, std::string header = "");

private:
	static void loadResources(TiXmlHandle projectH);
	static ParticleSystem loadParticleSystem(TiXmlElement* psystemE); // TODO: single psystem loading?

	// global resource info
	static bufferUmap globalBufferInfo;
	static atomicUmap globalAtomicInfo;
	static uniformUmap globalUniformInfo;
	
	// global resource loading functions
	static void loadGlobalResources(TiXmlHandle globalResH);
	static bool loadGlobalBuffers(TiXmlHandle buffers);
	static bool loadGlobalAtomics(TiXmlHandle atomics);
	static bool loadGlobalUniforms(TiXmlHandle uniforms);
	static void addGlobalProgramResources(atomicUmap &aum, bufferUmap &bum, uniformUmap &uum);
	static void loadIterationResourceOverrides(TiXmlElement* actionsElement, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum);


	// reserved resource info
	static std::vector<bufferInfo> reservedBufferInfo;
	static std::vector<atomicInfo> reservedAtomicInfo;
	static std::vector<uniformInfo> reservedUniformInfo;

	// reserved resource loading functions
	static void collectReservedResourceInfo(TiXmlHandle reservedResH);
	static bool loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemE);
	static void loadInitialResourceOverrides(reservedResources &rr, TiXmlElement* psystemE);
	static void loadReservedProgramResources(reservedResources &rr, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset);


	// program loading functions
	static bool loadComputeProgram(reservedResources &rr, TiXmlElement* programE, ComputeProgram &cp);
	static bool loadRenderer(reservedResources &rr, TiXmlElement* programE, RendererProgram &rp);
	static void getRendererXMLInfo(rendererLoading &rl, TiXmlElement* programE);

	// project loading utility functions
	static void collectFPaths(TiXmlElement* elem, const char *tag, std::vector<std::string> &target);
	static bool loadShader(GLuint program, GLuint shader, TiXmlElement* fPathE);
	
	static std::string createFinalShaderSource(std::vector<std::string> filePaths, std::string header = "");
	static std::string generateComputeHeader(atomicUmap &cpAtomicHandles, bufferUmap &cpBufferHandles, uniformUmap &cpUniforms);
	static std::string fileToString(std::string filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint program);
};

