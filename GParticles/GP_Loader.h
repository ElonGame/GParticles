#pragma once
#include <regex>
#include "GP_Systems.h"
#include "Texture.h"
#include "Model.h"
#include "Shader.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\tinystr.h"

struct reservedResources
{
	GLuint maxParticles;
	bufferUmap reservedBuffers;
	atomicUmap reservedAtomics;
	uniformUmap reservedUniforms;
	std::vector<glm::vec4> spheres;
	std::vector<glm::vec4> planes;
};

struct psProperties
{
	glm::mat4 model;
	GLuint lifetime;
	bool looping;
	GLuint numWorkGroups;
	
	psProperties() : model(glm::mat4()), lifetime(0), looping(true), numWorkGroups(1) {};
};

struct computeInfo
{
	std::vector<std::string> fPaths;
	std::vector<std::string> overrided;
	GLuint iterationStep;
};

struct renderInfo
{
	std::vector<std::string> vsPath;
	std::vector<std::string> fgPath;
	std::vector<std::string> gmPath;
	std::string rendertype;
	std::string path;
	GLuint iterationStep;
};

enum loadFunctionInfo
{
	NO_PREFAB,
	FIRST_ITERATION,
	SECOND_ITERATION
};

class GP_Loader
{
public:
	// parses file and fills psContainer with ParticleSystem objects
	static void loadProject(std::string filePath);

private:
	static void loadResources(TiXmlHandle projectH);
	static GP_ParticleSystem loadParticleSystem(TiXmlElement* psystemE);
	static void loadProperties(TiXmlElement* propertiesE, psProperties &psp, loadFunctionInfo lfi);
	
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
	static std::set<std::string> GP_Loader::getTags(TiXmlElement* stageE);

	static void collectReservedResourceInfo(TiXmlHandle reservedResH);
	static bool loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemE);
	static void collectColliders(reservedResources &rr, TiXmlElement* collidersE);
	static void loadInitialResourceOverrides(reservedResources &rr, TiXmlElement* psystemE);


	// program loading functions
	static AbstractProgram* loadComputeProgram(GLuint numWorkGroups, reservedResources &rr, std::set<std::string> &tags, TiXmlElement* stageE);
	static AbstractProgram* loadRenderProgram(reservedResources &rr, std::set<std::string> &tags, TiXmlElement* stageE);
	static void getRenderInfo(renderInfo &rl, TiXmlElement* stageE);

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


	static void collectPaths(TiXmlElement* elem, const char *tag, std::vector<std::string> &target);
	static bool loadShader(GLuint program, GLuint shader, TiXmlElement* fPathE);
	
	static std::string createFinalShaderSource(std::vector<std::string> fPaths, std::string psystemName);
	static std::string generateRenderHeader(bufferUmap &buffers, atomicUmap &atomics,uniformUmap &uniforms, std::string in, std::string out, std::string renderType);
	static std::string generateComputeHeader(bufferUmap &buffers, atomicUmap &atomics, uniformUmap &uniforms, std::vector<glm::vec4> spheres, std::vector<glm::vec4> planes);
	static std::string fileToString(std::string filePath);
	static std::string fillTemplate(std::string templateHeader, std::string psystemName);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint program);


	
};

