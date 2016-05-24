#include "ParticleSystemLoader.h"
#include "Shader.h"

bufferUmap ParticleSystemLoader::globalBufferInfo;
atomicUmap ParticleSystemLoader::globalAtomicInfo;
uniformUmap ParticleSystemLoader::globalUniformInfo;

std::vector<bufferInfo> ParticleSystemLoader::reservedBufferInfo;
std::vector<atomicInfo> ParticleSystemLoader::reservedAtomicInfo;
std::vector<uniformInfo> ParticleSystemLoader::reservedUniformInfo;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer)
{
	// open file and get doc handle
	TiXmlDocument doc(filePath);
	if (!doc.LoadFile())
	{
		printf("Failed to load file \"%s\"\n", doc);
		return false;
	}

	TiXmlHandle docHandle(&doc);
	TiXmlHandle projectH = docHandle.FirstChild("project");


	// load global project resources
	TiXmlHandle resourcesH = projectH.FirstChild("resources");
	loadGlobalBuffers(resourcesH.FirstChild("buffers"));
	loadGlobalAtomics(resourcesH.FirstChild("atomics"));
	loadGlobalUniforms(resourcesH.FirstChild("uniforms"));
	

	// collect reserved resource default info
	collectReservedResourceInfo();


	// load particle systems
	TiXmlElement* psystemE = projectH.FirstChildElement("psystem").ToElement();
	for (; psystemE; psystemE = psystemE->NextSiblingElement())
	{
		psContainer.push_back(loadParticleSystem(psystemE));
		psContainer.back().printContents();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ParticleSystem ParticleSystemLoader::loadParticleSystem(TiXmlElement* psystemE)
{
	// load reserved resources
	reservedResources rr;
	loadReservedPSResources(rr, psystemE);


	// load psystem main components (emitter, updater, renderer)
	TiXmlElement* programE;

	ComputeProgram emitter;
	programE = psystemE->FirstChildElement("emitter");
	if (!loadComputeProgram(rr, programE, emitter))
	{
		printf("Unable to load emitter!\n");
	}

	ComputeProgram updater;
	programE = psystemE->FirstChildElement("updater");
	if (!loadComputeProgram(rr, programE, updater))
	{
		printf("Unable to load updater!\n");
	}

	RendererProgram renderer;
	programE = psystemE->FirstChildElement("renderer");
	if (!loadRenderer(rr, programE, renderer))
	{
		printf("Unable to load renderer!\n");
	}


	// load psystem properties
	// model matrix
	glm::mat4 model = glm::mat4();
	glm::vec3 pos = glm::vec3();
	TiXmlElement* transformE = psystemE->FirstChildElement("position");
	transformE->QueryFloatAttribute("x", &pos.x);
	transformE->QueryFloatAttribute("y", &pos.y);
	transformE->QueryFloatAttribute("z", &pos.z);

	glm::vec3 rot = glm::vec3();
	transformE = psystemE->FirstChildElement("rotation");
	transformE->QueryFloatAttribute("x", &rot.x);
	transformE->QueryFloatAttribute("y", &rot.y);
	transformE->QueryFloatAttribute("z", &rot.z);
	float angle = 0;
	transformE->QueryFloatAttribute("angle", &angle);

	glm::vec3 scale = glm::vec3();
	transformE = psystemE->FirstChildElement("scale");
	transformE->QueryFloatAttribute("x", &scale.x);
	transformE->QueryFloatAttribute("y", &scale.y);
	transformE->QueryFloatAttribute("z", &scale.z);

	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(angle), rot);
	model = glm::scale(model, scale);

	// numWorkGroups
	GLuint numWorkGroups;
	TiXmlElement* nWorkGroupE = psystemE->FirstChildElement("numWorkGroups");
	nWorkGroupE->QueryUnsignedAttribute("value", &numWorkGroups);

	// lifetime
	unsigned int lifetime = 0;
	std::string str = "millisec";
	bool looping = false;
	TiXmlElement* lifetimeE = psystemE->FirstChildElement("lifetime");
	if (lifetimeE->QueryUnsignedAttribute("limit", &lifetime) == TIXML_SUCCESS)
	{
		if (lifetimeE->QueryStringAttribute("unit", &str) == TIXML_SUCCESS)
		{
			lifetime *= (str == "millisec") ? 1 : 1000;
		}

		if (lifetimeE->QueryStringAttribute("looping", &str) == TIXML_SUCCESS)
		{
			looping = (str == "true");
		}
	}

	// TODO: add more psystem porperties


	return ParticleSystem(emitter, updater, renderer, model, numWorkGroups, lifetime, looping);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGlobalBuffers(TiXmlHandle buffers)
{
	TiXmlElement* buffer = buffers.ToElement();
	if (!buffer)
	{
		printf("No global buffer resources found!\n");
		return false;
	}


	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	// iterate through every buffer resource
	buffer = buffer->FirstChildElement();
	for (; buffer; buffer = buffer->NextSiblingElement())
	{
		bufferInfo bi;

		// parse buffer info and store <name, bufferInfo> pair
		glGenBuffers(1, &bi.id);
		buffer->QueryUnsignedAttribute("elements", &bi.elements);
		buffer->QueryStringAttribute("name", &bi.name);
		buffer->QueryStringAttribute("type", &bi.type);

		globalBufferInfo.emplace(bi.name, bi);

		// initialize buffer
		int bSize = bi.elements * ((bi.type == "vec4") ? 4 : 1);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, globalBufferInfo.at(bi.name).id);
		glBufferData(	GL_SHADER_STORAGE_BUFFER, bSize * sizeof(GLfloat),
						NULL, GL_DYNAMIC_DRAW);

		// fill buffer with default values
		GLfloat *values = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bSize * sizeof(GLfloat), bufMask);
		
		for (int i = 0; i < bSize; values[i++] = 0);

		
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		std::cout << "(GLOBAL) INIT: buffer " << bi.name << " with number " << globalBufferInfo.at(bi.name).id
			<< ", " << bi.elements << " elements and size " << bSize << std::endl; // DUMP
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGlobalAtomics(TiXmlHandle atomics)
{
	TiXmlElement* atomic = atomics.ToElement();
	if (!atomic)
	{
		printf("No global atomic resources found!\n");
		return false;
	}

	// iterate through every atomic resource
	atomic = atomic->FirstChildElement();
	for (; atomic; atomic = atomic->NextSiblingElement())
	{
		atomicInfo ai;

		// parse atomic info and store <name, atomicInfo> pair
		glGenBuffers(1, &ai.id);
		atomic->QueryUnsignedAttribute("value", &ai.initialValue);
		atomic->QueryStringAttribute("name", &ai.name);

		globalAtomicInfo.emplace(ai.name, ai);

		// initialize atomic buffer
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ai.id);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &ai.initialValue);

		std::cout << "(GLOBAL) INIT: atomic " << ai.name << " with number " <<
			ai.id << " and starting value " << ai.initialValue << std::endl; // DUMP
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadGlobalUniforms(TiXmlHandle uniforms)
{
	TiXmlElement* uniform = uniforms.ToElement();
	if (!uniform)
	{
		printf("No global uniform resources found!\n");
		return false;
	}

	// iterate through every uniform resource
	uniform = uniform->FirstChildElement();
	for (; uniform; uniform = uniform->NextSiblingElement())
	{
		uniformInfo ui;

		// parse uniform and store <name, atomicInfo> pair
		uniform->QueryStringAttribute("name", &ui.name);
		uniform->QueryStringAttribute("type", &ui.type);
		uniform->QueryFloatAttribute("value", &ui.value);

		globalUniformInfo.emplace(ui.name, ui);

		std::cout << "(GLOBAL) INIT: uniform " << ui.name<< " of type " <<
			ui.type << " and value " << ui.value << std::endl; // DUMP
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::collectReservedResourceInfo()
{
	// open file and get resources info
	TiXmlDocument doc("reserved/reservedResources.xml");
	if (!doc.LoadFile())
	{
		printf("Failed to load file \"%s\"\n", doc);
		return false;
	}

	TiXmlHandle docHandle(&doc);
	TiXmlHandle resourcesH = docHandle.FirstChild("resources").ToElement();

	// collect buffer name, elements and type
	TiXmlElement* buffer = resourcesH.FirstChild("buffer").ToElement();
	for (; buffer; buffer = buffer->NextSiblingElement("buffer"))
	{
		bufferInfo bi;
		buffer->QueryStringAttribute("name", &bi.name);
		buffer->QueryUnsignedAttribute("elements", &bi.elements);
		buffer->QueryStringAttribute("type", &bi.type);

		reservedBufferInfo.push_back(bi);

		std::cout << "COLLECTED: buffer " << bi.name << ", with " << bi.elements
			<< " elements and type " << bi.type << std::endl; // DUMP
	}

	// collect atomic name and initialValue
	TiXmlElement* atomic = resourcesH.FirstChild("atomic").ToElement();
	for (; atomic; atomic = atomic->NextSiblingElement("atomic"))
	{
		atomicInfo ai;
		atomic->QueryStringAttribute("name", &ai.name);
		atomic->QueryUnsignedAttribute("value", &ai.initialValue);

		reservedAtomicInfo.push_back(ai);

		std::cout << "COLLECTED: atomic " << ai.name << " with initial value "
			<< ai.initialValue << std::endl; // DUMP
	}

	// collect uniform name, type and initial value
	TiXmlElement* uniform = resourcesH.FirstChild("uniform").ToElement();
	for (; uniform; uniform = uniform->NextSiblingElement("uniform"))
	{
		uniformInfo ui;
		uniform->QueryStringAttribute("name", &ui.name);
		uniform->QueryStringAttribute("type", &ui.type);
		uniform->QueryFloatAttribute("value", &ui.value);

		reservedUniformInfo.push_back(ui);

		std::cout << "COLLECTED: uniform " << ui.name << ", with initial value "
			<< ui.value << " and type " << ui.type << std::endl; // DUMP
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemE)
{
	// get psystem name
	std::string psystemName;
	int res = psystemE->QueryStringAttribute("name", &psystemName);
	if (res != TIXML_SUCCESS)
	{
		std::string msg = "Must give a name to psystem on line " + std::to_string(psystemE->Row());
		Utils::exitMessage("Invalid Input", msg);
	}

	// load default reserved atomics, uniforms and buffers
	for (auto resAtmInfo : reservedAtomicInfo)
	{
		atomicInfo ai = resAtmInfo;
		ai.name = psystemName + ai.name;
		ai.reset = false;
		
		rr.reservedAtomicInfo.emplace(ai.name, ai);
	}

	for (auto resUniInfo : reservedUniformInfo)
	{
		uniformInfo ui = resUniInfo;
		ui.name = psystemName + ui.name;

		rr.reservedUniformInfo.emplace(ui.name, ui);

		std::cout << "INIT: uniform " << ui.name << " of type " <<
			ui.type << " and value " << ui.value << std::endl; // DUMP
	}

	for (auto resBufInfo : reservedBufferInfo)
	{
		bufferInfo bi = resBufInfo;
		bi.name = psystemName + bi.name;

		rr.reservedBufferInfo.emplace(bi.name, bi);
	}


	// check and apply resource value overrides
	loadInitialResourceOverrides(rr, psystemE);


	// initialize reserved atomic buffers
	for (auto &ab : rr.reservedAtomicInfo)
	{
		glGenBuffers(1, &ab.second.id);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ab.second.id);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &ab.second.initialValue);

		std::cout << "INIT: atomic " << ab.second.name << " with number " <<
			ab.second.id << " and starting value " << ab.second.initialValue << std::endl; // DUMP
	}

	
	// get psystem max particles and initialize reserved buffers
	rr.maxParticles = rr.reservedUniformInfo.at(psystemName + "_maxParticles").value;
	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	for (auto &bi : rr.reservedBufferInfo)
	{
		if (rr.maxParticles != 0)
		{
			bi.second.elements = rr.maxParticles;
		}
	
		int multiplier = 1;
		if (bi.second.type == "vec4")
		{
			multiplier = 4;
		}
		else if (bi.second.type == "vec2")
		{
			multiplier = 2;
		}

		int bSize = bi.second.elements * multiplier;

		glGenBuffers(1, &bi.second.id);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bi.second.id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

		// fill buffer with default values
		GLfloat *values = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bSize * sizeof(GLfloat), bufMask);

		for (int i = 0; i < bSize; values[i++] = 0);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		std::cout << "INIT: buffer " << bi.second.name << " with number " << bi.second.id
			<< ", " << bi.second.elements << " elements and size " << bSize << std::endl; // DUMP
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::loadInitialResourceOverrides(reservedResources & rr, TiXmlElement * psystemE)
{
	TiXmlElement* resE = psystemE->FirstChildElement("override");
	std::string resType, resName;
	for (; resE; resE = resE->NextSiblingElement("override"))
	{
		resE->QueryStringAttribute("type", &resType);
		resE->QueryStringAttribute("name", &resName);

		if (resType == "uniform")
		{
			resE->QueryFloatAttribute("value",
				&rr.reservedUniformInfo.at(resName).value);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::addGlobalProgramResources(atomicUmap &aum, bufferUmap &bum, uniformUmap &uum)
{
	// add global atomics
	for (auto gah : globalAtomicInfo)
	{
		aum.emplace(gah);
	}

	// add global buffers
	for (auto gbh : globalBufferInfo)
	{
		bum.emplace(gbh);
	}

	// add global uniforms
	for (auto guh : globalUniformInfo)
	{
		uum.emplace(guh);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::loadIterationResourceOverrides(
	TiXmlElement * programE, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum)
{
	if (programE == nullptr) { return; }

	TiXmlElement* resE = programE->FirstChildElement("override");
	
	if (resE == nullptr) { return; }

	std::string resType, resName, resValue;
	for (; resE; resE = resE->NextSiblingElement())
	{
		resE->QueryStringAttribute("type", &resType);
		resE->QueryStringAttribute("name", &resName);
		resE->QueryStringAttribute("value", &resValue);

		if (resType == "atomic")
		{
			aum.at(resName).reset = true;
			aum.at(resName).initialValue = std::stoul(resValue, nullptr, 0);
		}
		else if (resType == "uniform")
		{ 
			uum.at(resName).value = std::stof(resValue);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadComputeProgram(reservedResources &rr, TiXmlElement* programE, ComputeProgram &cp)
{
	// parse and store program resource info for later binding
	// start by adding the program reserved resources
	atomicUmap cpAtomics = rr.reservedAtomicInfo;
	bufferUmap cpBuffers = rr.reservedBufferInfo;
	uniformUmap cpUniforms = rr.reservedUniformInfo;

	// then, add the program global resources
	cpAtomics.insert(globalAtomicInfo.begin(), globalAtomicInfo.end());
	cpBuffers.insert(globalBufferInfo.begin(), globalBufferInfo.end());
	cpUniforms.insert(globalUniformInfo.begin(), globalUniformInfo.end());

	// and mark the indicated resources to be reset every iteration
	loadIterationResourceOverrides(programE, cpAtomics, cpBuffers, cpUniforms);

	// create shader program
	GLuint cpHandle = glCreateProgram();

	// parse file paths that compose final shader
	std::vector<std::string> fPaths;
	collectFPaths(programE, "file", fPaths);

	// shader source = header (from resource info) + reserved functionality + filepaths source
	std::string shaderSource = generateComputeHeader(cpAtomics, cpBuffers, cpUniforms);
	shaderSource += fileToString("reserved/utilities.glsl");
	shaderSource += createFinalShaderSource(fPaths);

	// compile, attach and check link status
	Shader cpShader("compute", shaderSource);

	glAttachShader(cpHandle, cpShader.getId());
	glLinkProgram(cpHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(cpHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", cpHandle);
		printProgramLog(cpHandle);
		return false;
	}
	
	cp = ComputeProgram(cpHandle, cpAtomics, cpBuffers, cpUniforms, rr.maxParticles);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::getRendererXMLInfo(rendererLoading &rl, TiXmlElement* programE)
{
	if (programE == NULL)
		return;
	
	// renderType
	TiXmlElement* renderType = programE->FirstChildElement("rendertype");
	if (renderType != NULL && rl.rendertype.empty())
	{
		int res = renderType->QueryStringAttribute("type", &rl.rendertype);
		if (res != TIXML_SUCCESS)
		{
			std::string msg = "Must give a type to rendertype on line " + std::to_string(renderType->Row());
			Utils::exitMessage("Invalid Input", msg);
		}

		if (rl.rendertype == "billboard")
		{
			res = renderType->QueryStringAttribute("path", &rl.path);
			if (res != TIXML_SUCCESS)
			{
				std::string msg = "Must supply image path to billboard rendertype on line " + std::to_string(renderType->Row());
				Utils::exitMessage("Invalid Input", msg);
			}
		}

		if (rl.rendertype == "model")
		{
			res = renderType->QueryStringAttribute("path", &rl.path);
			if (res != TIXML_SUCCESS)
			{
				std::string msg = "Must supply directory path to model rendertype on line " + std::to_string(renderType->Row());
				Utils::exitMessage("Invalid Input", msg);
			}
		}
	}

	// collect shader file paths
	collectFPaths(programE, "vertfile", rl.vsPath);
	collectFPaths(programE, "geomfile", rl.gmPath);
	collectFPaths(programE, "fragfile", rl.fgPath);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ParticleSystemLoader::collectFPaths(TiXmlElement* elem, const char *tag, std::vector<std::string> &target)
{
	if (!target.empty())
		return;

	TiXmlElement* fPathE = elem->FirstChildElement(tag);
	if (fPathE == NULL)
		return;
	
	std::string fragmentPath;
	for (; fPathE; fPathE = fPathE->NextSiblingElement(tag))
	{
		fPathE->QueryStringAttribute("path", &fragmentPath);
		target.push_back(fragmentPath);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::loadRenderer(reservedResources &rr, TiXmlElement* programE, RendererProgram &rp)
{
	// fill rendererLoading struct with the final info to be processed
	// starting with the original file
	rendererLoading rl;
	getRendererXMLInfo(rl, programE);

	// check if there is a prefab and continue filling the the struct
	std::string st;
	programE->QueryStringAttribute("prefab", &st);
	
	TiXmlDocument doc(st);
	if (doc.LoadFile())
	{
		TiXmlHandle docHandle(&doc);
		programE = docHandle.FirstChild("prefab").ToElement();
		getRendererXMLInfo(rl, programE);
	}



	// parse and store program resource info for later binding
	//TiXmlElement* resourcesElement = programE->FirstChildElement("resources");
	// TODO: Fix this
	atomicUmap rendAtomics = rr.reservedAtomicInfo;
	bufferUmap rendBuffers = rr.reservedBufferInfo;
	uniformUmap rendUniforms = rr.reservedUniformInfo;
	std::vector<GLuint> rendAtmHandlesToReset;
	Texture texture;
	Model model;

	addGlobalProgramResources(rendAtomics, rendBuffers, rendUniforms);
	//addAtomicResets(resourcesElement, rendAtomics, rendAtmHandlesToReset);

	// create atomic binding points
	int bindingPoint = 0;
	std::vector<std::pair<GLuint, GLuint>> rendAtmHandles;
	for (auto atm : rendAtomics)
	{
		rendAtmHandles.push_back(std::make_pair(atm.second.id, bindingPoint++));
	}




	GLuint rpHandle = glCreateProgram();

	// vertex shader
	if (rl.vsPath.empty())
	{
		Utils::exitMessage("Invalid Input", "Must provide a file for vertex shader");
	}
	std::string source = createFinalShaderSource(rl.vsPath);
	Shader rpVertShader("vertex", source);
	rpVertShader.dumpToFile();
	glAttachShader(rpHandle, rpVertShader.getId());

	// geometry shader
	if (!rl.gmPath.empty())
	{
		source = createFinalShaderSource(rl.gmPath);
		Shader rpGeomShader("geometry", source);
		glAttachShader(rpHandle, rpGeomShader.getId());
	}

	// frag shader
	if (rl.fgPath.empty())
	{
		Utils::exitMessage("Invalid Input", "Must provide a file for fragment shader");
	}
	source = createFinalShaderSource(rl.fgPath);
	Shader rpFragShader("fragment", source);
	glAttachShader(rpHandle, rpFragShader.getId());



	glLinkProgram(rpHandle);

	GLint programSuccess = GL_FALSE;
	glGetProgramiv(rpHandle, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE)
	{
		printf("Error linking program %d!\n", rpHandle);
		printProgramLog(rpHandle);
		return false;
	}


	// renderType
	if (rl.rendertype == "billboard")
	{
		texture.setImage(rl.path);
	}
	else if (rl.rendertype == "model")
	{
		model = Model((GLchar*)rl.path.c_str());

	}


	// generate vertex array object
	GLuint vao, ebo, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (rl.rendertype == "model")
	{
		glGenBuffers(1, &ebo);
		glGenBuffers(1, &vbo);
		// model first mesh
		Mesh mesh = model.meshes[0];

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
			&mesh.vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(GLuint),
			&mesh.indices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glEnableVertexAttribArray(0);
		GLint posLocation = glGetAttribLocation(rpHandle, "position");
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

		// Vertex Normals
		glEnableVertexAttribArray(1);
		GLint norLocation = glGetAttribLocation(rpHandle, "normal");
		glVertexAttribPointer(norLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, normal));

		// Vertex TexCoords
		glEnableVertexAttribArray(2);
		GLint tcLocation = glGetAttribLocation(rpHandle, "texCoords");
		glVertexAttribPointer(tcLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, texCoords));
	}


	// add instance buffers to vao
	for (auto b : rendBuffers)
	{
		GLint location = glGetAttribLocation(rpHandle, b.first.c_str());
		glBindBuffer(GL_ARRAY_BUFFER, b.second.id);
		glEnableVertexAttribArray(location);
		GLuint elemType = (b.second.type == "float") ? 1 : 4;
		glVertexAttribPointer(location, elemType, GL_FLOAT, 0, 0, 0);

		if (rl.rendertype == "model")
		{
			// tell OpenGL this is an instanced vertex attribute
			glVertexAttribDivisor(location, 1); 
		}
	}

	glBindVertexArray(0);

	rp = RendererProgram(rpHandle, rendAtmHandles, vao, texture.getId(), rendUniforms, rl.rendertype, model);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ParticleSystemLoader::compileShaderFiles(GLuint shaderID, std::vector<std::string> fPaths, std::string header)
{
	// parse file fragments into string
	std::string shaderString = header;
	for (std::string fPath : fPaths)
	{
		shaderString = shaderString + fileToString(fPath);
	}

	// dump final file
	std::string name = "dumps/" + std::to_string(shaderID) + "_shader.comp";
	std::ofstream out(name);
	out << shaderString;
	out.close();

	// compile shader files
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
std::string ParticleSystemLoader::createFinalShaderSource(std::vector<std::string> fPaths, std::string header)
{
	// parse file fragments into string
	std::string shaderString = header;
	for (std::string fPath : fPaths)
	{
		shaderString = shaderString + fileToString(fPath);
	}
	return shaderString;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string ParticleSystemLoader::generateComputeHeader(atomicUmap &cpAtomicHandles, bufferUmap &cpBufferHandles, uniformUmap &cpUniforms)
{
	std::string res =	"#version 430\n"
						"#extension GL_ARB_compute_shader : enable\n"
						"#extension GL_ARB_shader_storage_buffer_object : enable\n"
						"#extension GL_ARB_compute_variable_group_size : enable\n\n"
						"////////////////////////////////////////////////////////////////////////////////\n"
						"// RESOURCES\n"
						"////////////////////////////////////////////////////////////////////////////////\n\n";

	int i = 0;
	for (auto &atm : cpAtomicHandles)
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

	res += "uniform int timet;\n";
	res += "uniform vec2 mouse;\n";

	// TODO: put this on reserved header
	res += "\nuint gid = gl_GlobalInvocationID.x;\n";

	return res;
}