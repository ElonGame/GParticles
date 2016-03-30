#include "ParticleSystemLoader.h"


bufferUmap ParticleSystemLoader::globalBufferHandles;
atomicUmap ParticleSystemLoader::globalAtomicHandles;
uniformUmap ParticleSystemLoader::globalUniformHandles;

std::vector<bufferInfo> ParticleSystemLoader::reservedBufferInfo;
std::vector<atomicInfo> ParticleSystemLoader::reservedAtomicInfo;
std::vector<uniformInfo> ParticleSystemLoader::reservedUniformInfo;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer)
{
	// open file and get first element handle
	TiXmlDocument doc(filePath);
	if (!doc.LoadFile())
	{
		printf("Failed to load file \"%s\"\n", doc);
		return false;
	}

	TiXmlHandle docHandle(&doc);
	TiXmlElement* projectElement = docHandle.FirstChildElement("project").ToElement();
	

	// collect reserved resource default info
	collectReservedResourceInfo();


	// load global project resources
	TiXmlElement* resourcesElement = projectElement->FirstChildElement("resources");
	if (!loadGlobalBuffers(resourcesElement->FirstChildElement("buffers")))
	{
		printf("Unable to initialize buffers!\n");
		return false;
	}

	if (!loadGlobalAtomics(resourcesElement->FirstChildElement("atomics")))
	{
		printf("Unable to initialize atomics!\n");
		return false;
	}

	if (!loadGlobalUniforms(resourcesElement->FirstChildElement("uniforms")))
	{
		printf("Unable to initialize uniforms!\n");
		return false;
	}


	// load particle systems
	TiXmlElement* psystemElement = projectElement->FirstChildElement("psystem");
	for (; psystemElement; psystemElement = psystemElement->NextSiblingElement())
	{
		psContainer.push_back(loadParticleSystem(psystemElement));
		psContainer.back().printContents();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ParticleSystem ParticleSystemLoader::loadParticleSystem(TiXmlElement* psystemElement)
{
	reservedResources rr;
	psystemElement->QueryStringAttribute("name", &rr.psystemName);

	// load reserved resources
	loadReservedPSResources(rr);


	TiXmlElement* program;
	// load emitter
	ComputeProgram emitter;
	program = psystemElement->FirstChildElement("emitter");
	if (!loadComputeProgram(rr, program, emitter))
	{
		printf("Unable to load emitter!\n");
	}


	// load updater
	ComputeProgram updater;
	program = psystemElement->FirstChildElement("updater");
	if (!loadComputeProgram(rr, program, updater))
	{
		printf("Unable to load updater!\n");
	}


	// load renderer
	RendererProgram renderer;
	if (!loadRenderer(psystemElement->FirstChildElement("renderer"), renderer))
	{
		printf("Unable to load renderer!\n");
	}

	// transform
	glm::mat4 model = glm::mat4();
	glm::vec3 pos = glm::vec3();
	TiXmlElement* transElement = psystemElement->FirstChildElement("position");
	transElement->QueryFloatAttribute("x", &pos.x);
	transElement->QueryFloatAttribute("y", &pos.y);
	transElement->QueryFloatAttribute("z", &pos.z);

	glm::vec3 rot = glm::vec3();
	transElement = psystemElement->FirstChildElement("rotation");
	transElement->QueryFloatAttribute("x", &rot.x);
	transElement->QueryFloatAttribute("y", &rot.y);
	transElement->QueryFloatAttribute("z", &rot.z);
	float angle = 0;
	transElement->QueryFloatAttribute("angle", &angle);

	glm::vec3 scale = glm::vec3();
	transElement = psystemElement->FirstChildElement("scale");
	transElement->QueryFloatAttribute("x", &scale.x);
	transElement->QueryFloatAttribute("y", &scale.y);
	transElement->QueryFloatAttribute("z", &scale.z);

	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(angle), rot);
	model = glm::scale(model, scale);

	// lifetime
	unsigned int lifetime = 0;
	std::string str = "millisec";
	bool looping = false;
	TiXmlElement* lifetimeElem = psystemElement->FirstChildElement("lifetime");
	if (lifetimeElem->QueryUnsignedAttribute("limit", &lifetime) == TIXML_SUCCESS)
	{
		if (lifetimeElem->QueryStringAttribute("unit", &str) == TIXML_SUCCESS)
		{
			lifetime *= (str == "millisec") ? 1 : 1000;
		}

		if (lifetimeElem->QueryStringAttribute("looping", &str) == TIXML_SUCCESS)
		{
			looping = (str == "true");
		}
	}

	// TODO: add more psystem porperties


	return ParticleSystem(emitter, updater, renderer, model, lifetime, looping);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGlobalBuffers(TiXmlElement * buffers)
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

		globalBufferHandles.emplace(bName, bi);

		// initialize buffer
		int bSize = bi.elements * ((bi.type == "vec4") ? 4 : 1);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, globalBufferHandles.at(bName).id);
		glBufferData(	GL_SHADER_STORAGE_BUFFER, bSize * sizeof(GLfloat),
						NULL, GL_DYNAMIC_DRAW);

		// fill buffer with default values
		GLfloat *values = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bSize * sizeof(GLfloat), bufMask);
		for (int i = 0; i < bSize; values[i++] = 0);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		std::cout << "loaded buffer " << bName << " with number " << globalBufferHandles.at(bName).id
			<< ", " << bi.elements << " elements and size " << bSize << std::endl; // DUMP
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGlobalAtomics(TiXmlElement * atomics)
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

		globalAtomicHandles.emplace(atmName, ai);

		// initialize atomic buffer
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ai.id);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &ai.initialValue);

		std::cout << "loaded atomic " << atmName << " with number " <<
			ai.id << " and starting value " << ai.initialValue << std::endl;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGlobalUniforms(TiXmlElement * uniforms)
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

		globalUniformHandles.emplace(uName, ui);

		std::cout << "loaded uniform " << uName<< " of type " <<
			ui.type << " and value " << ui.value << std::endl;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::collectReservedResourceInfo()
{
	// open file and get resources element handle
	TiXmlDocument doc("reserved/reservedResources.xml");
	if (!doc.LoadFile())
	{
		printf("Failed to load file \"%s\"\n", doc);
		return false;
	}

	TiXmlHandle docHandle(&doc);
	TiXmlElement* resourcesElement = docHandle.FirstChildElement("resources").ToElement();

	// collect buffer name, elements and type
	TiXmlElement* buffer = resourcesElement->FirstChildElement("buffers")->FirstChildElement();
	for (; buffer; buffer = buffer->NextSiblingElement())
	{
		bufferInfo bi;
		buffer->QueryStringAttribute("name", &bi.name);
		buffer->QueryUnsignedAttribute("elements", &bi.elements);
		buffer->QueryStringAttribute("type", &bi.type);

		reservedBufferInfo.push_back(bi);

		std::cout << "RESERVED: collected buffer " << bi.name << ", with " << bi.elements << " elements and type " << bi.type << std::endl;
	}

	// collect atomic name and initialValue
	TiXmlElement* atomic = resourcesElement->FirstChildElement("atomics")->FirstChildElement();
	for (; atomic; atomic = atomic->NextSiblingElement())
	{
		atomicInfo ai;
		atomic->QueryStringAttribute("name", &ai.name);
		atomic->QueryUnsignedAttribute("value", &ai.initialValue);

		reservedAtomicInfo.push_back(ai);

		std::cout << "RESERVED: collected atomic " << ai.name << " with initial value " << ai.initialValue << std::endl;
	}

	// collect uniform name, type and initial value
	TiXmlElement* uniform = resourcesElement->FirstChildElement("uniforms")->FirstChildElement();
	for (; uniform; uniform = uniform->NextSiblingElement())
	{
		uniformInfo ui;
		uniform->QueryStringAttribute("name", &ui.name);
		uniform->QueryStringAttribute("type", &ui.type);
		uniform->QueryFloatAttribute("value", &ui.value);

		reservedUniformInfo.push_back(ui);

		std::cout << "RESERVED: collected uniform " << ui.name << ", with initial value " << ui.value << " and type " << ui.type << std::endl;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadReservedPSResources(reservedResources &rr)
{
	// load reserved atomics
	for each (auto resAtmInfo in reservedAtomicInfo)
	{
		atomicInfo ai = resAtmInfo;

		glGenBuffers(1, &ai.id);
		std::string atmName = "res_" + rr.psystemName + ai.name;
		
		rr.reservedAtomicHandles.emplace(atmName, ai);

		// initialize atomic buffer
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ai.id);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &ai.initialValue);

		std::cout << "RESERVED: initalized atomic " << atmName << " with number " <<
			ai.id << " and starting value " << ai.initialValue << std::endl;
	}

	// load reserved buffers
	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	for each (auto resBufInfo in reservedBufferInfo)
	{
		bufferInfo bi = resBufInfo;

		glGenBuffers(1, &bi.id);
		std::string bName = "res_" + rr.psystemName + bi.name;

		rr.reservedBufferHandles.emplace(bName, bi);

		// initialize buffer
		int bSize = bi.elements * ((bi.type == "vec4") ? 4 : 1);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bi.id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

		// fill buffer with default values
		GLfloat *values = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bSize * sizeof(GLfloat), bufMask);
		for (int i = 0; i < bSize; values[i++] = 0);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		std::cout << "RESERVED: initialized buffer " << bName << " with number " << bi.id
			<< ", " << bi.elements << " elements and size " << bSize << std::endl; // DUMP
	}

	// load reserved uniforms
	for each (auto resUniInfo in reservedUniformInfo)
	{
		uniformInfo ui = resUniInfo;

		std::string uniName = "res_" + rr.psystemName + ui.name;

		rr.reservedUniformHandles.emplace(uniName, ui);

		std::cout << "RESERVED: initialized uniform " << uniName << " of type " <<
			ui.type << " and value " << ui.value << std::endl;
	}

	return true;
}

void ParticleSystemLoader::loadReservedProgramResources(reservedResources &rr, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint>& aToReset)
{
	// atomics
	for each (auto reservedAtm in rr.reservedAtomicHandles)
	{
		aum.emplace(reservedAtm.first, reservedAtm.second);
	}

	// buffers
	for each (auto reservedBuf in rr.reservedBufferHandles)
	{
		bum.emplace(reservedBuf.first, reservedBuf.second);
	}

	// buffers
	for each (auto reservedUni in rr.reservedUniformHandles)
	{
		uum.emplace(reservedUni.first, reservedUni.second);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::loadGlobalProgramResources(TiXmlElement* resources, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum, std::vector<GLuint> &aToReset)
{
	// atomics
	std::string atmName;
	TiXmlElement* atomic = resources->FirstChildElement("atomics")->FirstChildElement();
	for (; atomic; atomic = atomic->NextSiblingElement())
	{
		atomic->QueryStringAttribute("name", &atmName);
		aum.emplace(atmName, globalAtomicHandles.at(atmName));

		std::string resetValue;
		if (atomic->QueryStringAttribute("reset", &resetValue) == TIXML_SUCCESS)
		{
			if (resetValue == "true")
				aToReset.push_back(aum.at(atmName).id);
		}
	}

	// buffers
	std::string bName;
	TiXmlElement* buffer = resources->FirstChildElement("buffers")->FirstChildElement();
	for (; buffer; buffer = buffer->NextSiblingElement())
	{
		buffer->QueryStringAttribute("name", &bName);

		bum.emplace(bName, globalBufferHandles.at(bName));
	}

	// uniforms
	std::string uName;
	TiXmlElement* uniform = resources->FirstChildElement("uniforms")->FirstChildElement();
	for (; uniform; uniform = uniform->NextSiblingElement())
	{
		uniform->QueryStringAttribute("name", &uName);

		uum.emplace(uName, globalUniformHandles.at(uName));
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadComputeProgram(reservedResources &rr, TiXmlElement* glpElement, ComputeProgram &glp)
{
	// parse and store program resource handles for later binding
	TiXmlElement* resourcesElement = glpElement->FirstChildElement("resources");
	atomicUmap cpAtomicHandles;
	bufferUmap cpBufferHandles;
	uniformUmap cpUniforms;
	std::vector<GLuint> cpAtmHandlesToReset;

	loadReservedProgramResources(rr, cpAtomicHandles, cpBufferHandles, cpUniforms, cpAtmHandlesToReset);
	loadGlobalProgramResources(resourcesElement, cpAtomicHandles, cpBufferHandles, cpUniforms, cpAtmHandlesToReset);


	GLuint cpHandle = glCreateProgram();
	GLuint cpShader = glCreateShader(GL_COMPUTE_SHADER);

	// parse file fragment paths that compose final shader
	std::vector<std::string> fPaths;
	std::string fragmentPath;

	TiXmlElement* fPathElement = glpElement->FirstChildElement("files")->FirstChildElement();
	for (; fPathElement; fPathElement = fPathElement->NextSiblingElement())
	{
		fPathElement->QueryStringAttribute("path", &fragmentPath);

		fPaths.push_back(fragmentPath);
	}

	// generate shader header from resource info
	std::string header = generateHeader(cpAtomicHandles, cpBufferHandles, cpUniforms);
	// fetch reserved functionality
	// TODO: add more
	std::string reservedFunctions = fileToString("reserved/noise2D.glsl") + fileToString("reserved/utilities.glsl");// +fileToString("reserved/generators.glsl");

	// compile, attach and check link status
	compileShaderFiles(cpShader, header, reservedFunctions, fPaths, true);
	glAttachShader(cpHandle, cpShader);
	glLinkProgram(cpHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(cpHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", cpHandle);
		printProgramLog(cpHandle);
		return false;
	}

	// create GLProgram, storing only the handles of buffers and atomics
	std::vector<std::pair<GLuint, GLuint>> cpAtmHandles;
	for each (auto atm in cpAtomicHandles)
	{
		cpAtmHandles.push_back(std::make_pair(atm.second.id, atm.second.binding));
	}

	std::vector<std::pair<GLuint, GLuint>> cpBHandles;
	for each (auto b in cpBufferHandles)
	{
		cpBHandles.push_back(std::make_pair(b.second.id, b.second.binding));
	}

	glp = ComputeProgram(cpHandle, cpAtmHandles, cpAtmHandlesToReset, cpBHandles, cpUniforms);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadRenderer(TiXmlElement* glpElement, RendererProgram &rp)
{
	// parse and store program resource handles for later binding
	TiXmlElement* resourcesElement = glpElement->FirstChildElement("resources");
	atomicUmap rendAtomicHandles;
	bufferUmap rendBufferHandles;
	uniformUmap rendUniforms;
	std::vector<GLuint> rendAtmHandlesToReset;

	loadGlobalProgramResources(resourcesElement, rendAtomicHandles, rendBufferHandles, rendUniforms, rendAtmHandlesToReset);


	GLuint rpHandle = glCreateProgram();

	// vertex shader
	GLuint rpVertShader = glCreateShader(GL_VERTEX_SHADER);

	// parse file fragments
	std::vector<std::string> vertFrags;
	std::string fragmentPath;

	TiXmlElement* fPathElement = glpElement->FirstChildElement("vertfiles")->FirstChildElement();
	for (; fPathElement; fPathElement = fPathElement->NextSiblingElement())
	{
		fPathElement->QueryStringAttribute("path", &fragmentPath);

		vertFrags.push_back(fragmentPath);
	}

	// compile, attach
	compileShaderFiles(rpVertShader, "", "", vertFrags, true);
	glAttachShader(rpHandle, rpVertShader);


	// frag shader
	GLuint rpFragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// parse file fragments
	std::vector<std::string> fragFrags;

	fPathElement = glpElement->FirstChildElement("fragfiles")->FirstChildElement();
	for (; fPathElement; fPathElement = fPathElement->NextSiblingElement())
	{
		fPathElement->QueryStringAttribute("path", &fragmentPath);

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
	std::vector<std::pair<GLuint, GLuint>> rendAtmHandles;
	for each (auto atm in rendAtomicHandles)
	{
		rendAtmHandles.push_back(std::make_pair(atm.second.id, atm.second.binding));
	}

	// generate vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// add buffers to vao
	for each (auto b in rendBufferHandles)
	{
		GLint location = glGetAttribLocation(rpHandle, b.first.c_str());
		glBindBuffer(GL_ARRAY_BUFFER, b.second.id);
		glEnableVertexAttribArray(location);
		GLuint elemType = (b.second.type == "float") ? 1 : 4;
		glVertexAttribPointer(location, elemType, GL_FLOAT, 0, 0, 0);
	}

	rp = RendererProgram(rpHandle, rendAtmHandles, vao, rendUniforms);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::compileShaderFiles(GLuint shaderID, std::string header, std::string reservedFunctions, std::vector<std::string> fPaths, bool dumpToFile)
{
	// parse file fragments into string
	std::string shaderString = header + reservedFunctions;
	for each (std::string fPath in fPaths)
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
std::string ParticleSystemLoader::generateHeader(atomicUmap &glpAtomicHandles, bufferUmap &cpBufferHandles, uniformUmap &cpUniforms)
{
	std::string res =	"#version 430\n"
						"#extension GL_ARB_compute_shader : enable\n"
						"#extension GL_ARB_shader_storage_buffer_object : enable\n"
						"#extension GL_ARB_compute_variable_group_size : enable\n\n"
						"////////////////////////////////////////////////////////////////////////////////\n"
						"// RESOURCES\n"
						"////////////////////////////////////////////////////////////////////////////////\n\n";

	int i = 0;
	for (auto &atm : glpAtomicHandles)
	{
		atm.second.binding = i;

		res +=	"layout(binding = " + std::to_string(i++) +
				", offset = 0) uniform atomic_uint " + atm.first +";\n";
	}

	res += "\n";

	for (auto &buf : cpBufferHandles)
	{
		buf.second.binding = i;

		std::string aux = buf.first;
		aux[0] = toupper(aux[0]);

		res +=	"layout(std430, binding = " + std::to_string(i++) + ") buffer " + aux + "\n{\n"
				"\t" + buf.second.type + " " + buf.first + "[];\n};\n\n";
	}

	res += "layout(local_size_variable) in;\n\n";

	for (auto &uni : cpUniforms)
	{
		res += "uniform " + uni.second.type + " " + uni.first + ";\n";
	}

	// TODO: put this on reserved header
	res += "\nuint gid = gl_GlobalInvocationID.x;\n";

//	std::cout << res << std::endl;

	return res;
}