#include "ParticleSystemLoader.h"

ParticleSystemLoader::ParticleSystemLoader()
{
}

ParticleSystemLoader::~ParticleSystemLoader()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ParticleSystem ParticleSystemLoader::loadParticleSystem(std::string filePath)
{

	// TODO: correct -> element handle as parameter
	// TODO: instead of finding the resources every time, pass a resource object
	// open file and get first element handle
	TiXmlDocument doc(filePath);
	if (!doc.LoadFile())
	{
		printf("Failed to load file \"%s\"\n", doc);
	}

	TiXmlHandle docHandle(&doc);
	TiXmlHandle projectHandle = docHandle.FirstChildElement("project");
	TiXmlHandle resourcesHandle = projectHandle.FirstChildElement("resources");
	TiXmlHandle psystemHandle = projectHandle.FirstChildElement("psystem");

	if (!initBuffers(resourcesHandle.FirstChildElement("buffers").ToElement()))
	{
		printf("Unable to initialize buffers!\n");
	}

	if (!initAtomics(resourcesHandle.FirstChildElement("atomics").ToElement()))
	{
		printf("Unable to initialize atomics!\n");
	}

	if (!initUniforms(resourcesHandle.FirstChildElement("uniforms").ToElement()))
	{
		printf("Unable to initialize uniforms!\n");
	}


	// load emitter
	ComputeProgram emitter;
	if (!loadGLProgram(psystemHandle.FirstChildElement("emitter").ToElement(), emitter))
	{
		printf("Unable to load emitter!\n");
	}


	// load updater
	ComputeProgram updater;
	if (!loadGLProgram(psystemHandle.FirstChildElement("updater").ToElement(), updater))
	{
		printf("Unable to load updater!\n");
	}


	// load Renderer
	RendererProgram renderer;
	if (!loadRenderer(psystemHandle.FirstChildElement("renderer").ToElement(), renderer))
	{
		printf("Unable to load renderer!\n");
	}

	return ParticleSystem(emitter, updater, renderer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::initBuffers(TiXmlElement * buffers)
{
	if (!buffers)
	{
		printf("No buffer resources found!\n");
		return false;
	}

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;


	// iterate through every buffer resource
	TiXmlElement* buffer = buffers->FirstChildElement();
	for (; buffer; buffer = buffer->NextSiblingElement())
	{
		bufferInfo bi;

		// parse buffer info and store <name, bufferInfo> pair
		glGenBuffers(1, &bi.id);
		buffer->QueryUnsignedAttribute("elements", &bi.elements);
		buffer->QueryStringAttribute("type", &bi.type);

		std::string bName;
		buffer->QueryStringAttribute("name", &bName);

		bufferHandles.emplace(bName, bi);

		// initialize buffer
		int bSize = bi.elements * ((bi.type == "vec4") ? 4 : 1);
		std::cout << "elements:::: " << bi.elements << "  bSize:::" << bSize << std::endl;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferHandles.at(bName).id);
		glBufferData(	GL_SHADER_STORAGE_BUFFER, bSize * sizeof(GLfloat),
						NULL, GL_DYNAMIC_DRAW);

		// fill buffer with default values
		std::cout << " --Buffer " << bufferHandles.at(bName).id << std::endl;
		GLfloat *values = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bSize * sizeof(GLfloat), bufMask);
		for (int i = 0; i < bSize; values[i++] = 0);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		std::cout << "created buffer " << bName <<
			" with number " << bufferHandles.at(bName).id << std::endl;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::initAtomics(TiXmlElement * atomics)
{
	if (!atomics)
	{
		printf("No atomic resources found!\n");
		return false;
	}

	// iterate through every atomic resource
	TiXmlElement* atomic = atomics->FirstChildElement();
	for (; atomic; atomic = atomic->NextSiblingElement())
	{
		atomicInfo ai;

		// parse atomic info and store <name, atomicInfo> pair
		glGenBuffers(1, &ai.id);
		atomic->QueryUnsignedAttribute("value", &ai.initialValue);

		std::string atmName;
		atomic->QueryStringAttribute("name", &atmName);

		atomicHandles.emplace(atmName, ai);

		// initialize atomic buffer
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ai.id);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &ai.initialValue);

		std::cout << "created atomic " << atmName << " with number " <<
			ai.id << " and starting value " << ai.initialValue << std::endl;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::initUniforms(TiXmlElement * uniforms)
{
	if (!uniforms)
	{
		printf("No uniform resources found!\n");
		return false;
	}

	// iterate through every uniform resource
	TiXmlElement* uniform = uniforms->FirstChildElement();
	for (; uniform; uniform = uniform->NextSiblingElement())
	{
		uniformInfo ui;

		// parse uniform and store <name, atomicInfo> pair
		uniform->QueryFloatAttribute("value", &ui.value);
		uniform->QueryStringAttribute("type", &ui.type);
		std::string uName;
		uniform->QueryStringAttribute("name", &uName);

		uniformHandles.emplace(uName, ui);

		std::cout << "stored uniform " << uName<< " of type " <<
			ui.type << " and value " << ui.value << std::endl;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadRenderer(TiXmlElement* glpElement, RendererProgram &rp)
{
	// parse and store glprogram resource handles for later binding
	TiXmlElement* resourcesHandle = glpElement->FirstChildElement("resources");

	// atomics
	atomicUmap glpAtomicHandles;
	std::string atmName;
	TiXmlElement* atomic = resourcesHandle->FirstChildElement("atomics")->FirstChildElement();
	for (; atomic; atomic = atomic->NextSiblingElement())
	{
		atomic->QueryStringAttribute("name", &atmName);

		glpAtomicHandles.emplace(atmName, atomicHandles.at(atmName));
	}

	// buffers
	bufferUmap glpBufferHandles;
	std::string bName;
	TiXmlElement* buffer = resourcesHandle->FirstChildElement("buffers")->FirstChildElement();
	for (; buffer; buffer = buffer->NextSiblingElement())
	{
		buffer->QueryStringAttribute("name", &bName);

		glpBufferHandles.emplace(bName, bufferHandles.at(bName));
	}

	// uniforms
	uniformUmap glpUniforms;
	std::string uName;
	TiXmlElement* uniform = resourcesHandle->FirstChildElement("uniforms")->FirstChildElement();
	for (; uniform; uniform = uniform->NextSiblingElement())
	{
		uniform->QueryStringAttribute("name", &uName);

		glpUniforms.emplace(uName, uniformHandles.at(uName));
	}


	GLuint rpHandle = glCreateProgram();

	// vertex shader
	GLuint rpVertShader = glCreateShader(GL_VERTEX_SHADER);

	// parse file fragments
	std::vector<std::string> vertFrags;
	std::string fragmentPath;

	TiXmlElement* fileFrag = glpElement->FirstChildElement("vertfiles")->FirstChildElement();
	for (; fileFrag; fileFrag = fileFrag->NextSiblingElement())
	{
		fileFrag->QueryStringAttribute("path", &fragmentPath);

		vertFrags.push_back(fragmentPath);
	}

	// compile, attach
	compileShaderFiles(rpVertShader, "", "", vertFrags, true);
	glAttachShader(rpHandle, rpVertShader);


	// frag shader
	GLuint rpFragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// parse file fragments
	std::vector<std::string> fragFrags;

	fileFrag = glpElement->FirstChildElement("fragfiles")->FirstChildElement();
	for (; fileFrag; fileFrag = fileFrag->NextSiblingElement())
	{
		fileFrag->QueryStringAttribute("path", &fragmentPath);

		fragFrags.push_back(fragmentPath);
	}

	// compile, attach and check link status
	compileShaderFiles(rpFragShader, "", "", fragFrags, true);
	glAttachShader(rpHandle, rpFragShader);

	glLinkProgram(rpHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(rpHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", rpHandle);
		printProgramLog(rpHandle);
		return false;
	}

	// get correct atomics
	std::vector<GLuint> glpAtmHandles;
	for each (auto atm in glpAtomicHandles)
	{
		glpAtmHandles.push_back(atm.second.id);
	}

	// generate vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// add buffers to vao
	for each (auto b in glpBufferHandles)
	{
		GLint location = glGetAttribLocation(rpHandle, b.first.c_str());
		glBindBuffer(GL_ARRAY_BUFFER, b.second.id);
		glEnableVertexAttribArray(location);
		GLuint elemType = (b.second.type == "float") ? 1 : 4;
		glVertexAttribPointer(location, elemType, GL_FLOAT, 0, 0, 0);
	}

	rp = RendererProgram(rpHandle, glpAtmHandles, vao, glpUniforms);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGLProgram(TiXmlElement* glpElement, ComputeProgram &glp)
{

	// parse and store glprogram resource handles for later binding
	TiXmlElement* resourcesHandle = glpElement->FirstChildElement("resources");

	// atomics
	atomicUmap glpAtomicHandles;
	std::vector<GLuint> glpAtmHandlesToReset;
	std::string atmName;
	TiXmlElement* atomic = resourcesHandle->FirstChildElement("atomics")->FirstChildElement();
	for (; atomic; atomic = atomic->NextSiblingElement())
	{
		atomic->QueryStringAttribute("name", &atmName);

		glpAtomicHandles.emplace(atmName, atomicHandles.at(atmName));

		std::string resetValue;
		if (atomic->QueryStringAttribute("reset", &resetValue) == TIXML_SUCCESS)
		{
			if (resetValue == "true")
				glpAtmHandlesToReset.push_back(atomicHandles.at(atmName).id);
		}
	}

	// buffers
	bufferUmap glpBufferHandles;
	std::string bName;
	TiXmlElement* buffer = resourcesHandle->FirstChildElement("buffers")->FirstChildElement();
	for (; buffer; buffer = buffer->NextSiblingElement())
	{
		buffer->QueryStringAttribute("name", &bName);

		glpBufferHandles.emplace(bName, bufferHandles.at(bName));
	}

	// uniforms
	uniformUmap glpUniforms;
	std::string uName;
	TiXmlElement* uniform = resourcesHandle->FirstChildElement("uniforms")->FirstChildElement();
	for (; uniform; uniform = uniform->NextSiblingElement())
	{
		uniform->QueryStringAttribute("name", &uName);

		glpUniforms.emplace(uName, uniformHandles.at(uName));
	}


	GLuint glpHandle = glCreateProgram();
	GLuint glpShader = glCreateShader(GL_COMPUTE_SHADER);

	// parse file fragments
	std::vector<std::string> fFrags;
	std::string fragmentPath;

	TiXmlElement* fileFrag = glpElement->FirstChildElement("files")->FirstChildElement();
	for (; fileFrag; fileFrag = fileFrag->NextSiblingElement())
	{
		fileFrag->QueryStringAttribute("path", &fragmentPath);

		fFrags.push_back(fragmentPath);
	}

	// generate shader header from resource info
	std::string header = generateHeader(glpAtomicHandles, glpBufferHandles, glpUniforms);
	std::string reservedFunctions = fileToString("reserved/noise2D.glsl");

	// compile, attach and check link status
	compileShaderFiles(glpShader, header, reservedFunctions, fFrags, true);
	glAttachShader(glpHandle, glpShader);
	glLinkProgram(glpHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(glpHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", glpHandle);
		printProgramLog(glpHandle);
		return false;
	}

	// create GLProgram
	std::vector<GLuint> glpAtmHandles;
	for each (auto atm in glpAtomicHandles)
	{
		glpAtmHandles.push_back(atm.second.id);
	}

	std::vector<GLuint> glpBHandles;
	for each (auto b in glpBufferHandles)
	{
		glpBHandles.push_back(b.second.id);
	}

	glp = ComputeProgram(glpHandle, glpAtmHandles, glpAtmHandlesToReset, glpBHandles, glpUniforms);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::compileShaderFiles(GLuint shaderID, std::string header, std::string reservedFunctions, std::vector<std::string> fFrags, bool dumpToFile)
{
	// parse file fragments into string
	std::string shaderString = header + reservedFunctions;
	for each (std::string fPath in fFrags)
	{
		shaderString = shaderString + fileToString(fPath);
	}

	if (dumpToFile)
	{
		std::string name = "dumps/" + std::to_string(shaderID) + "_shader.comp";
		std::ofstream out(name);
		out << shaderString;
		out.close();
	}

	const GLchar* shaderSource = shaderString.c_str();
	glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(shaderID);

	GLint cShaderCompiled = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &cShaderCompiled);
	if (cShaderCompiled != GL_TRUE)
	{
		printf("Unable to compile compute shader %d!\n", shaderID);
		printShaderLog(shaderID);
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string ParticleSystemLoader::fileToString(std::string filePath)
{
	std::string str;
	std::ifstream sourceFile(filePath.c_str());
	if (sourceFile)
	{
		str.assign((std::istreambuf_iterator< char >(sourceFile)),
			std::istreambuf_iterator< char >());
	}

	return str;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::printShaderLog(GLuint shader)
{
	// check if input is really a shader
	if (glIsShader(shader))
	{
		int infoLogLength = 0;
		int maxLength = 0;

		// get info string maxlength
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];

		// get info log
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			printf("%s\n", infoLog);
		}

		// deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a shader\n", shader);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::printProgramLog(GLuint program)
{
	// check if input is really a program
	if (glIsProgram(program))
	{
		int infoLogLength = 0;
		int maxLength = 0;

		// get info string max length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];

		// get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			printf("%s\n", infoLog);
		}

		// deallocate string
		delete[] infoLog;
	}
	else
	{
		printf("Name %d is not a program\n", program);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string ParticleSystemLoader::generateHeader(atomicUmap glpAtomicHandles, bufferUmap glpBufferHandles, uniformUmap glpUniforms)
{
	std::string res =	"#version 430\n"
						"#extension GL_ARB_compute_shader : enable\n"
						"#extension GL_ARB_shader_storage_buffer_object : enable\n"
						"#extension GL_ARB_compute_variable_group_size : enable\n\n";

	int i = 0;
	// TODO: reserved atomics
	for each (auto atm in glpAtomicHandles)
	{
		res +=	"layout(binding = " + std::to_string(i++) +
				", offset = 0) uniform atomic_uint " + atm.first +";\n";
	}

	res += "\n";

	// TODO: reserved buffers
	for each(auto buf in glpBufferHandles)
	{
		std::string aux = buf.first;
		aux[0] = toupper(aux[0]);

		res +=	"layout(std430, binding = " + std::to_string(i++) + ") buffer " + aux + "\n{\n"
				"\t" + buf.second.type + " " + buf.first + "[];\n};\n\n";
	}

	res += "layout(local_size_variable) in;\n\n";

	for each (auto uni in glpUniforms)
	{
		res += "uniform " + uni.second.type + " " + uni.first + ";\n";
	}

	res += "\n";

	return res;
}