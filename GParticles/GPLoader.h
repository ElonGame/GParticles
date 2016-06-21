#pragma once
#include <fstream>
#include "ParticleSystem.h"
#include "Texture.h"
#include "Model.h"
#include "Shader.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"
#include "GP_Uniform.h"
#include "GP_Atomic.h"

struct reservedResources
{
	GLuint maxParticles;
	bufferUmap reservedBuffers;
	atomicUmap reservedAtomics;
	uniformUmap reservedUniforms;
};

struct renderLoading
{
	std::vector<std::string> vsPath;
	std::vector<std::string> fgPath;
	std::vector<std::string> gmPath;
	std::string rendertype;
	std::string path;
};

class GPLoader
{
public:
	// parses file and fills psContainer with ParticleSystem objects
	static bool loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer);

private:
	static void loadResources(TiXmlHandle projectH);
	static ParticleSystem loadParticleSystem(TiXmlElement* psystemE); // TODO: single psystem loading?
	
	// global resource info
	static bufferUmap globalBufferInfo;
	static atomicUmap globalAtomicInfo;
	static uniformUmap globalUniformInfo;

	// reserved resource info
	static std::vector<GP_Buffer> reservedBufferInfo;
	static std::vector<GP_Atomic> reservedAtomicInfo;
	static std::vector<GP_Uniform> reservedUniformInfo;

	// resource loading functions
	static void loadGlobalBuffers(TiXmlHandle globalResH);
	static void loadGlobalAtomics(TiXmlHandle globalResH);
	static void loadGlobalUniforms(TiXmlHandle globalResH);
	static void loadIterationResourceOverrides(TiXmlElement* actionsElement, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum);
	static void queryUniformValue(TiXmlElement* uElem, GP_Uniform &u);

	static void collectReservedResourceInfo(TiXmlHandle reservedResH);
	static bool loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemE);
	static void loadInitialResourceOverrides(reservedResources &rr, TiXmlElement* psystemE);
	static void loadReservedProgramResources(reservedResources &rr, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset);


	// program loading functions
	static bool loadComputeProgram(reservedResources &rr, TiXmlElement* eventE, ComputeProgram &cp);
	static bool loadRenderProgram(reservedResources &rr, TiXmlElement* eventE, RendererProgram &rp);
	static void getRenderXMLInfo(renderLoading &rl, TiXmlElement* eventE);

	// project loading utility functions
	template <typename Func>
	static void queryAttribute(Func func, TiXmlElement* elem, std::string errorMsg, std::string errorMsgTitle = "Invalid Input")
	{
		int res = func(elem);
		if (res != TIXML_SUCCESS)
		{
			Utils::exitMessage(	errorMsgTitle + " - line " +
								std::to_string(elem->Row()), errorMsg);
		}
	}


	static void collectFPaths(TiXmlElement* elem, const char *tag, std::vector<std::string> &target);
	static bool loadShader(GLuint program, GLuint shader, TiXmlElement* fPathE);
	
	static std::string createFinalShaderSource(std::vector<std::string> filePaths);
	static std::string generateRenderHeader(bufferUmap &buffers, atomicUmap &atomics,uniformUmap &uniforms,
											std::string in, std::string out);
	static std::string generateComputeHeader(bufferUmap &buffers, atomicUmap &atomics, uniformUmap &uniforms);
	static std::string fileToString(std::string filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint program);


	
};

