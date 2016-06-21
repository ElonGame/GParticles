#include "GP_Loader.h"

bufferUmap GP_Loader::globalBufferInfo;
atomicUmap GP_Loader::globalAtomicInfo;
uniformUmap GP_Loader::globalUniformInfo;
std::vector<GP_Buffer> GP_Loader::reservedBufferInfo;
std::vector<GP_Atomic> GP_Loader::reservedAtomicInfo;
std::vector<GP_Uniform> GP_Loader::reservedUniformInfo;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Loader::loadProject(std::string filePath, std::vector<ParticleSystem> &psContainer)
{
	// get tinyxml handle for project file
	TiXmlDocument doc(filePath);
	if (!doc.LoadFile())
	{
		printf("Failed to load file \"%s\"\n", filePath.c_str());
		return false;
	}

	TiXmlHandle docH(&doc);
	TiXmlHandle projectH = docH.FirstChild("project");
	if (!projectH.Element())
	{
		Utils::exitMessage("Invalid Input", "Input file must have an outer project tag");
	}


	// load global resources and store reserved resources info
	loadResources(projectH.FirstChild("resources"));


	 
	 /*std::ofstream out("dumps/templateTEEEEEST.glsl");
	 out << fillTemplate("templates/updateMain.glsl", "test");
	 out.close();*/

	// iterate through and load particle systems
	TiXmlElement* psystemE = projectH.FirstChildElement("psystem").ToElement();
	for (; psystemE; psystemE = psystemE->NextSiblingElement())
	{
		psContainer.push_back(loadParticleSystem(psystemE));
		psContainer.back().printContents();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadResources(TiXmlHandle resourcesH)
{
	auto lambda = [](TiXmlHandle globalResH, TiXmlHandle reservedResH)
	{	// load global project resources
		loadGlobalBuffers(globalResH);
		loadGlobalAtomics(globalResH);
		loadGlobalUniforms(globalResH);
		
		// collect reserved resource default info
		collectReservedResourceInfo(reservedResH);
	};

	// start by processing current file
	lambda(resourcesH.FirstChild("global"), resourcesH.FirstChild("private"));


	// if there is a resources prefab continue loading/collecting resources
	std::string st;
	resourcesH.ToElement()->QueryStringAttribute("prefab", &st);

	TiXmlDocument doc(st);
	if (doc.LoadFile())
	{
		TiXmlHandle docH(&doc);
		TiXmlHandle prefabH = docH.FirstChild("prefab").FirstChild("resources");
		
		lambda(prefabH.FirstChild("global"), prefabH.FirstChild("private"));
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadGlobalBuffers(TiXmlHandle globalResH)
{
	// skip function if there are no buffer tags
	TiXmlElement* bufferE = globalResH.FirstChild("buffer").ToElement();
	if (!bufferE)
	{
		return;
	}

	// iterate through every buffer tag
	for (; bufferE; bufferE = bufferE->NextSiblingElement("buffer"))
	{
		GP_Buffer b;

		// parse name and skip buffer loading if another has already been loaded with that name
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryStringAttribute("name", &b.name);},
						bufferE, "Must provide buffer attribute with a name");

		if (GlobalData::get().getBuffer(b.name, b))
		{
			printf(	"Skipping loading of buffer %s! Another buffer with the "
					"same name has already been loaded!\n", b.name);

			continue;
		}

		// parse buffer info, initialize and store it
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryUnsignedAttribute("elements", &b.elements);},
						bufferE, "Must provide buffer attribute with number of elements");
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryStringAttribute("type", &b.type);},
						bufferE, "Must provide buffer attribute with a type");
		
		b.init();

		globalBufferInfo.emplace(b.name, b);
		GlobalData::get().addBuffer(b);

		std::cout << "(GLOBAL) INIT: buffer " << b.name << " with number " << b.id
			<< ", " << b.elements << " elements of type " << b.type << std::endl; // DUMP
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadGlobalAtomics(TiXmlHandle globalResH)
{
	// skip function if there are no atomic tags
	TiXmlElement* atomicE = globalResH.FirstChild("atomic").ToElement();
	if (!atomicE)
	{
		return;
	}

	// iterate through every atomic resource
	for (; atomicE; atomicE = atomicE->NextSiblingElement("atomic"))
	{
		GP_Atomic a;
		
		// parse name and skip atomic loading if another has already been loaded with that name
		queryAttribute(	[&a](TiXmlElement* e) {return e->QueryStringAttribute("name", &a.name);},
						atomicE, "Must provide atomic attribute with a name");
		
		if (GlobalData::get().getAtomic(a.name, a))
		{
			printf("Skipping loading of atomic %s! Another atomic with the "
				"same name has already been loaded!\n", a.name);

			continue;
		}

		// parse atomic info, initialize and store it
		queryAttribute(	[&a](TiXmlElement* e) {return e->QueryUnsignedAttribute("value", &a.resetValue);},
						atomicE, "Must provide atomic attribute with a value > 0");

		a.init();

		globalAtomicInfo.emplace(a.name, a);
		GlobalData::get().addAtomic(a);

		std::cout << "(GLOBAL) INIT: atomic " << a.name << " with number " <<
			a.id << " and starting value " << a.resetValue << std::endl; // DUMP
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::queryUniformValue(TiXmlElement* uElem, GP_Uniform &u)
{
	//if (u.type == "bool")
	//{
	//	int res = uElem->QueryBoolAttribute( ("value", (GLuint *)&u.value[0].x);
	//	if (res != TIXML_SUCCESS || isnan(u.value[0].x))
	//	{
	//		std::string msg = "Must provide a value > -1 for uniform \"" + u.name
	//			+ "\" on line " + std::to_string(uElem->Row());
	//		Utils::exitMessage("Invalid Input", msg);
	//	}
	//}
	if (u.type == "uint")
	{
		int res = uElem->QueryUnsignedAttribute("value", (GLuint *)&u.value[0].x);
		if (res != TIXML_SUCCESS || isnan(u.value[0].x))
		{
			std::string msg = "Must provide a value > -1 for uniform \"" + u.name
							  + "\" on line " + std::to_string(uElem->Row());
			Utils::exitMessage("Invalid Input", msg);
		}
	}
	else if (u.type == "float")
	{
		uElem->QueryFloatAttribute("value", &u.value[0].x);
	}
	else if (u.type == "vec2")
	{
		uElem->QueryFloatAttribute("x", &u.value[0].x);
		uElem->QueryFloatAttribute("y", &u.value[0].y);
	}
	else if (u.type == "vec4")
	{
		uElem->QueryFloatAttribute("x", &u.value[0].x);
		uElem->QueryFloatAttribute("y", &u.value[0].y);
		uElem->QueryFloatAttribute("z", &u.value[0].z);
		uElem->QueryFloatAttribute("w", &u.value[0].w);
	}
	else if (u.type == "mat4")
	{
		for (int i = 0; i < 4; i++)
		{
			std::string l = std::to_string(i);
			std::vector<std::string> attribute = { "x"+l, "y"+l, "z"+l, "w"+l };
			uElem->QueryFloatAttribute(attribute[0].c_str(), &u.value[i].x);
			uElem->QueryFloatAttribute(attribute[1].c_str(), &u.value[i].y);
			uElem->QueryFloatAttribute(attribute[2].c_str(), &u.value[i].z);
			uElem->QueryFloatAttribute(attribute[3].c_str(), &u.value[i].w);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadGlobalUniforms(TiXmlHandle globalResH)
{
	// skip function if there are no uniform tags
	TiXmlElement* uniformE = globalResH.FirstChild("uniform").ToElement();
	if (!uniformE)
	{
		return;
	}

	// iterate through every uniform resource
	for (; uniformE; uniformE = uniformE->NextSiblingElement("uniform"))
	{
		GP_Uniform u;

		// parse name and skip uniform loading if another has already been loaded with that name
		queryAttribute(	[&u](TiXmlElement* e) {return e->QueryStringAttribute("name", &u.name);},
						uniformE, "Must provide uniform attribute with a name");

		if (GlobalData::get().getUniform(u.name, u))
		{
			printf("Skipping loading of uniform %s! Another uniform with the "
				"same name has already been loaded!\n", u.name);

			continue;
		}

		// parse atomic info, initialize and store it
		queryAttribute(	[&u](TiXmlElement* e) {return e->QueryStringAttribute("type", &u.type);},
						uniformE, "Must provide uniform attribute with a type");
		queryUniformValue(uniformE, u);

		globalUniformInfo.emplace(u.name, u);
		GlobalData::get().addUniform(u);

		std::cout << "(GLOBAL) INIT: uniform " << u.name<< " of type " <<
			u.type << " and value " << u.value[0].x << std::endl; // DUMP
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::collectReservedResourceInfo(TiXmlHandle reservedResH)
{
	bool skipIter = false;

	// collect buffer info
	TiXmlElement* bufferE = reservedResH.FirstChild("buffer").ToElement();
	for (; bufferE; bufferE = bufferE->NextSiblingElement("buffer"))
	{
		GP_Buffer b;

		// parse name and skip buffer loading if another has already been loaded with that name
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryStringAttribute("name", &b.name);},
						bufferE, "Must provide buffer attribute with a name");

		for (int i = 0; i < reservedBufferInfo.size() && !skipIter; i++)
		{
			if (reservedBufferInfo[i].name == b.name)
				skipIter = true;
		}

		if (skipIter)
		{
			skipIter = false;
			continue;
		}

		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryUnsignedAttribute("elements", &b.elements);},
						bufferE, "Must provide buffer attribute with number of elements");
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryStringAttribute("type", &b.type);},
						bufferE, "Must provide buffer attribute with a type");

		reservedBufferInfo.push_back(b);

		std::cout << "COLLECTED: buffer " << b.name << ", with " << b.elements
			<< " elements and type " << b.type << std::endl; // DUMP
	}

	// collect atomic info
	TiXmlElement* atomicE = reservedResH.FirstChild("atomic").ToElement();
	for (; atomicE; atomicE = atomicE->NextSiblingElement("atomic"))
	{
		GP_Atomic a;

		// parse name and skip atomic loading if another has already been loaded with that name
		queryAttribute(	[&a](TiXmlElement* e) {return e->QueryStringAttribute("name", &a.name);},
						atomicE, "Must provide atomic attribute with a name");

		for (int i = 0; i < reservedAtomicInfo.size() && !skipIter; i++)
		{
			if (reservedAtomicInfo[i].name == a.name)
				skipIter = true;
		}

		if (skipIter)
		{
			skipIter = false;
			continue;
		}

		atomicE->QueryUnsignedAttribute("value", &a.resetValue);

		reservedAtomicInfo.push_back(a);

		std::cout << "COLLECTED: atomic " << a.name << " with initial value "
			<< a.resetValue << std::endl; // DUMP
	}

	// collect uniform info
	TiXmlElement* uniformE = reservedResH.FirstChild("uniform").ToElement();
	for (; uniformE; uniformE = uniformE->NextSiblingElement("uniform"))
	{
		GP_Uniform u;

		// parse name and skip uniform loading if another has already been loaded with that name
		queryAttribute(	[&u](TiXmlElement* e) {return e->QueryStringAttribute("name", &u.name);},
						uniformE, "Must provide uniform attribute with a name");
		for (int i = 0; i < reservedUniformInfo.size() && !skipIter; i++)
		{
			if (reservedUniformInfo[i].name == u.name)
				skipIter = true;
		}

		if (skipIter)
		{
			skipIter = false;
			continue;
		}

		queryAttribute(	[&u](TiXmlElement* e) {return e->QueryStringAttribute("type", &u.type);},
						uniformE, "Must provide uniform attribute with a type");
		queryUniformValue(uniformE, u);

		reservedUniformInfo.push_back(u);

		std::cout << "COLLECTED: uniform " << u.name << ", with initial value "
			<< u.value[0].x << " and type " << u.type << std::endl; // DUMP
	}
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ParticleSystem GP_Loader::loadParticleSystem(TiXmlElement* psystemE)
{
	// load reserved resources
	reservedResources rr;
	loadReservedPSResources(rr, psystemE);

	TiXmlElement* eventsE = psystemE->FirstChildElement("events");
	if (!eventsE)
	{
		Utils::exitMessage("Fatal Error", "psystem must have an events tag");
	}

	// load psystem events (emission, update, render)
	ComputeProgram emission;
	TiXmlElement* eventE = eventsE->FirstChildElement("emission");
	if (!loadComputeProgram(rr, eventE, emission))
	{
		Utils::exitMessage("Fatal Error", "Unable to load emission event");
	}

	ComputeProgram update;
	eventE = eventsE->FirstChildElement("update");
	if (!loadComputeProgram(rr, eventE, update))
	{
		Utils::exitMessage("Fatal Error", "Unable to load update event");
	}

	RendererProgram render;
	eventE = eventsE->FirstChildElement("render");
	if (!loadRenderProgram(rr, eventE, render))
	{
		Utils::exitMessage("Fatal Error", "Unable to load render event");
	}


	// load psystem properties
	// model matrix
	glm::mat4 model = glm::mat4();
	glm::vec3 pos = glm::vec3();
	TiXmlElement* transformE = psystemE->FirstChildElement("position");
	if (transformE)
	{
		transformE->QueryFloatAttribute("x", &pos.x);
		transformE->QueryFloatAttribute("y", &pos.y);
		transformE->QueryFloatAttribute("z", &pos.z);
	}

	float angle = 0;
	glm::vec3 rot = glm::vec3();
	transformE = psystemE->FirstChildElement("rotation");
	if (transformE)
	{
		transformE->QueryFloatAttribute("x", &rot.x);
		transformE->QueryFloatAttribute("y", &rot.y);
		transformE->QueryFloatAttribute("z", &rot.z);
		transformE->QueryFloatAttribute("angle", &angle);
	}

	glm::vec3 scale = glm::vec3();
	transformE = psystemE->FirstChildElement("scale");
	if (transformE)
	{
		transformE->QueryFloatAttribute("x", &scale.x);
		transformE->QueryFloatAttribute("y", &scale.y);
		transformE->QueryFloatAttribute("z", &scale.z);
	}

	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(angle), rot);
	model = glm::scale(model, scale);


	// lifetime
	unsigned int lifetime;
	std::string unit;
	bool looping = true;
	TiXmlElement* lifetimeE = psystemE->FirstChildElement("lifetime");
	if (!lifetimeE)
	{
		Utils::exitMessage("Fatal Error", "psystem must have a lifetime tag");
	}

	queryAttribute(	[&](TiXmlElement* e) {return e->QueryUnsignedAttribute("value", &lifetime);},
					lifetimeE, "Must provide lifetime tag with a value attribute");

	queryAttribute(	[&](TiXmlElement* e) {return e->QueryStringAttribute("unit", &unit);},
					lifetimeE, "Must provide lifetime tag with a unit attribute");

	// looping attribute is optional, defaults to true
	lifetimeE->QueryBoolAttribute("looping", &looping);

	if (unit == "seconds")
	{
		lifetime *= 1000;
	}
	else if (unit == "milliseconds");
	else
	{
		Utils::exitMessage("Invalide Input", "Unrecognized time unit. Supported time units are: seconds, milliseconds");
	}


	// numWorkGroups
	GLuint numWorkGroups = 1;
	TiXmlElement* nWorkGroupE = psystemE->FirstChildElement("numWorkGroups");
	if (nWorkGroupE)
	{
		nWorkGroupE->QueryUnsignedAttribute("value", &numWorkGroups);
	}


	return ParticleSystem(emission, update, render, model, numWorkGroups, lifetime, looping);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Loader::loadReservedPSResources(reservedResources &rr, TiXmlElement* psystemE)
{
	// get psystem name
	std::string psystemName;
	queryAttribute(	[&psystemName](TiXmlElement* e) {return e->QueryStringAttribute("name", &psystemName);},
					psystemE, "Must provide psystem tag with a name");

	for (auto resUniInfo : reservedUniformInfo)
	{
		GP_Uniform u = resUniInfo;
		
		std::string originalname = u.name;
		u.name = psystemName + "_" + u.name;

		rr.reservedUniforms.emplace(originalname, u);
		GlobalData::get().addUniform(u);
	}

	for (auto resAtmInfo : reservedAtomicInfo)
	{
		GP_Atomic a = resAtmInfo;

		std::string originalname = a.name;
		a.name = psystemName + "_" + a.name;

		a.init();

		rr.reservedAtomics.emplace(originalname, a);
		GlobalData::get().addAtomic(a);
	}

	// check and apply resource value overrides
	loadInitialResourceOverrides(rr, psystemE);

	// load default reserved atomics, uniforms and buffers
	rr.maxParticles = rr.reservedUniforms.at("maxParticles").value[0].x;
	for (auto resBufInfo : reservedBufferInfo)
	{
		GP_Buffer b = resBufInfo;
		std::string originalName = b.name;
		b.name = psystemName + "_" + b.name;
		
		b.init(rr.maxParticles);

		rr.reservedBuffers.emplace(originalName, b);
		GlobalData::get().addBuffer(b);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// TODO: support uniform and atomic overriding (only uniform for now)
void GP_Loader::loadInitialResourceOverrides(reservedResources & rr, TiXmlElement * psystemE)
{
	TiXmlElement* resE = psystemE->FirstChildElement("override");
	std::string resType, resName;
	for (; resE; resE = resE->NextSiblingElement("override"))
	{
		resE->QueryStringAttribute("name", &resName);
		// TODO: check res type
		queryUniformValue(resE, rr.reservedUniforms.at(resName));

		GlobalData::get().addUniform(rr.reservedUniforms.at(resName));
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadIterationResourceOverrides(
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
			aum.at(resName).resetValue = std::stoul(resValue, nullptr, 0);
			GlobalData::get().addAtomic(aum.at(resName));
		}
		else if (resType == "uniform")
		{ 
			uum.at(resName).value[0].x = std::stof(resValue);
			GlobalData::get().addUniform(uum.at(resName));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Loader::loadComputeProgram(reservedResources &rr, TiXmlElement* eventE, ComputeProgram &cp)
{
	// parse and store program resource info for later binding
	// start by adding the program reserved resources
	atomicUmap cpAtomics = rr.reservedAtomics;
	bufferUmap cpBuffers = rr.reservedBuffers;
	uniformUmap cpUniforms = rr.reservedUniforms;

	// then, add the program global resources
	cpBuffers.insert(globalBufferInfo.begin(), globalBufferInfo.end());
	cpAtomics.insert(globalAtomicInfo.begin(), globalAtomicInfo.end());
	cpUniforms.insert(globalUniformInfo.begin(), globalUniformInfo.end());

	// and mark the indicated resources to be reset every iteration
	loadIterationResourceOverrides(eventE, cpAtomics, cpBuffers, cpUniforms);


	// create shader program
	GLuint cpHandle = glCreateProgram();

	// parse file paths that compose final shader
	std::vector<std::string> fPaths;
	collectFPaths(eventE, "file", fPaths);

	// shader source = resource header + filepaths source + optional main logic
	std::string shaderSource = generateComputeHeader(cpBuffers, cpAtomics, cpUniforms);
	shaderSource += createFinalShaderSource(fPaths);

	std::string psystemName;
	eventE->Parent()->Parent()->ToElement()->QueryStringAttribute("name", &psystemName);

	std::string templatePath;
	TiXmlElement* templateE = eventE->FirstChildElement("mainTemplate");
	if (templateE)
	{
		int res = templateE->QueryStringAttribute("path", &templatePath);
		if (res == TIXML_SUCCESS)
		{
			shaderSource += fillTemplate(templatePath, psystemName);
		}
	}

	// compile, attach and check link status
	Shader cpShader("compute", shaderSource, psystemName + "_" + eventE->ValueStr());

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
	
	// shaderprogram only requires the resource headers
	resHeader bHeaders;
	for (auto b : cpBuffers)
	{
		bHeaders.push_back(std::make_pair(b.second.name, b.second.binding));
	}

	resHeader aHeaders;
	for (auto a : cpAtomics)
	{
		aHeaders.push_back(std::make_pair(a.second.name, a.second.binding));
	}

	std::vector<std::string> uHeaders;
	for (auto u : cpUniforms)
	{
		uHeaders.push_back(u.second.name);
	}

	GLuint iterationStep = 0;
	eventE->QueryUnsignedAttribute("iterationStep", &iterationStep);

	cp = ComputeProgram(cpHandle, bHeaders, aHeaders, uHeaders, psystemName, rr.maxParticles, iterationStep);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::getRenderXMLInfo(renderLoading &rl, TiXmlElement* eventE)
{
	if (eventE == NULL)
		return;
	
	// renderType
	TiXmlElement* renderTypeE = eventE->FirstChildElement("rendertype");
	if (renderTypeE != NULL && rl.rendertype.empty())
	{
		queryAttribute(	[&](TiXmlElement* e) {return e->QueryStringAttribute("type", &rl.rendertype);},
						renderTypeE, "Must provide rendertype with a type");

		queryAttribute(	[&](TiXmlElement* e) {return e->QueryStringAttribute("path", &rl.path);},
						renderTypeE, "Must provide rendertype with a resource path");
	}

	// collect shader file paths
	collectFPaths(eventE, "vertfile", rl.vsPath);
	collectFPaths(eventE, "geomfile", rl.gmPath);
	collectFPaths(eventE, "fragfile", rl.fgPath);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::collectFPaths(TiXmlElement* elem, const char *tag, std::vector<std::string> &target)
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
bool GP_Loader::loadRenderProgram(reservedResources &rr, TiXmlElement* eventE, RendererProgram &rp)
{
	// fill renderLoading struct with the final info to be processed
	// starting with the original file
	renderLoading rl;
	getRenderXMLInfo(rl, eventE);

	// check if there is a prefab and continue filling the the struct
	std::string st;
	eventE->QueryStringAttribute("prefab", &st);
	
	TiXmlDocument doc(st);
	if (doc.LoadFile())
	{
		TiXmlHandle docHandle(&doc);
		eventE = docHandle.FirstChild("prefab").ToElement();
		getRenderXMLInfo(rl, eventE);
	}



	// parse and store program resource info for later binding
	atomicUmap rendAtomics = rr.reservedAtomics;
	bufferUmap rendBuffers = rr.reservedBuffers;
	uniformUmap rendUniforms = rr.reservedUniforms;

	// then, add the program global resources
	rendBuffers.insert(globalBufferInfo.begin(), globalBufferInfo.end());
	rendAtomics.insert(globalAtomicInfo.begin(), globalAtomicInfo.end());
	rendUniforms.insert(globalUniformInfo.begin(), globalUniformInfo.end());


	// TODO: move to header generation
	// create atomic binding points
	int bindingPoint = 0;
	for (auto &atm : rendAtomics)
	{
		atm.second.binding = bindingPoint++;
	}



	// create shader program
	GLuint rpHandle = glCreateProgram();

	auto loadAndAttach = [&](auto type, auto paths, auto in, auto out)
	{
		std::string name;
		eventE->Parent()->Parent()->ToElement()->QueryStringAttribute("name", &name);
		std::string header = generateRenderHeader(rendBuffers, rendAtomics, rendUniforms, in, out);
		Shader shader(type, header + createFinalShaderSource(paths), name + "_" + eventE->ValueStr());
		glAttachShader(rpHandle, shader.getId());
	};

	// vertex shader
	if (rl.vsPath.empty())
	{
		Utils::exitMessage("Invalid Input", "Must provide a file for vertex shader");
	}
	loadAndAttach("vertex", rl.vsPath, "", "V");

	std::string fragIn = "V";

	// geometry shader
	if (!rl.gmPath.empty())
	{
		loadAndAttach("geometry", rl.gmPath, "V[]", "G");

		fragIn = "G";
	}

	// frag shader
	if (rl.fgPath.empty())
	{
		Utils::exitMessage("Invalid Input", "Must provide a file for fragment shader");
	}
	loadAndAttach("fragment", rl.fgPath, fragIn, "");


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
	Texture texture;
	Model model;
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

		// vertex Positions
		glEnableVertexAttribArray(0);
		GLint posLocation = glGetAttribLocation(rpHandle, "position");
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)0);

		// vertex Normals
		glEnableVertexAttribArray(1);
		GLint norLocation = glGetAttribLocation(rpHandle, "normal");
		glVertexAttribPointer(norLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, normal));

		// vertex TexCoords
		glEnableVertexAttribArray(2);
		GLint tcLocation = glGetAttribLocation(rpHandle, "texCoords");
		glVertexAttribPointer(tcLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, texCoords));
	}


	// add instance buffers to vao
	for (auto b : rendBuffers)
	{
		GLint location = glGetAttribLocation(rpHandle, b.second.name.c_str());
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

	resHeader aHeaders;
	for (auto a : rendAtomics)
	{
		aHeaders.push_back(std::make_pair(a.second.name, a.second.binding));
	}

	std::vector<std::string> uHeaders;
	for (auto u : rendUniforms)
	{
		uHeaders.push_back(u.second.name);
	}

	glBindVertexArray(0);

	rp = RendererProgram(rpHandle, aHeaders, vao, texture.getId(), uHeaders, rl.rendertype, model, rr.maxParticles);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GP_Loader::fileToString(std::string filePath)
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
void GP_Loader::printShaderLog(GLuint shader)
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
void GP_Loader::printProgramLog(GLuint program)
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
std::string GP_Loader::createFinalShaderSource(std::vector<std::string> fPaths)
{
	// parse file fragments into string
	std::string shaderString = "";
	for (std::string fPath : fPaths)
	{
		shaderString = shaderString + fileToString(fPath);
	}
	return shaderString;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GP_Loader::generateRenderHeader(bufferUmap &buffers, atomicUmap &atomics,
							uniformUmap &uniforms, std::string in, std::string out)
{
	std::string res =	"#version 430\n\n"
						"////////////////////////////////////////////////////////////////////////////////\n"
						"// RESOURCES\n"
						"////////////////////////////////////////////////////////////////////////////////\n\n";

	// atomic binding points need to be created first since their max value is 8
	int i = 0;
	for (auto &atm : atomics)
	{
		atm.second.binding = i++;

		res += "layout(binding = " + std::to_string(i) +
				", offset = 0) uniform atomic_uint " + atm.second.name + ";\n";
	}

	res += "\n";

	// buffers
	std::string bufferOuts = "";
	for (auto &buf : buffers)
	{
		buf.second.binding = i++;

		res += "in " + buf.second.type + " " + buf.second.name + in + ";\n";
		bufferOuts += "out " + buf.second.type + " " + buf.second.name + out + ";\n";
	}

	// fragment shader has no predefined outputs
	bufferOuts = (out != "") ? (bufferOuts) : "";

	res += "\n" + bufferOuts + "uniform mat4 model, view, projection;\n\n";

	// uniforms
	for (auto uni : uniforms)
	{
		res += "uniform " + uni.second.type + " " + uni.second.name + ";\n";
	}

	return res + "\n";
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GP_Loader::fillTemplate(std::string templatePath, std::string psystemName)
{
	std::string mainStr = fileToString(templatePath);
	std::string chr = "@";
	std::string to = psystemName + "_";

	size_t start_pos = 0;
	while ((start_pos = mainStr.find(chr, start_pos)) != std::string::npos)
	{
		mainStr.replace(start_pos, chr.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}

	return "\n\n" + mainStr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GP_Loader::generateComputeHeader(bufferUmap &buffers, atomicUmap &atomics, uniformUmap &uniforms)
{
	std::string res =	"#version 430\n"
						"#extension GL_ARB_compute_shader : enable\n"
						"#extension GL_ARB_shader_storage_buffer_object : enable\n"
						"#extension GL_ARB_compute_variable_group_size : enable\n\n"
						"////////////////////////////////////////////////////////////////////////////////\n"
						"// RESOURCES\n"
						"////////////////////////////////////////////////////////////////////////////////\n\n";

	// atomic binding points need to be created first since their max value is 8
	int i = 0;
	for (auto &atm : atomics)
	{
		atm.second.binding = i;

		res +=	"layout(binding = " + std::to_string(i++) +
				", offset = 0) uniform atomic_uint " + atm.second.name +";\n";
	}

	res += "\n";

	// buffers
	for (auto &buf : buffers)
	{
		buf.second.binding = i;

		std::string upperName = buf.second.name;
		upperName[0] = toupper(upperName[0]);

		res +=	"layout(std430, binding = " + std::to_string(i++) + ") buffer "
				+ upperName + "\n{\n\t" + buf.second.type + " " + buf.second.name
				+ "[];\n};\n\n";
	}

	res += "layout(local_size_variable) in;\n\n";

	// uniforms
	for (auto &uni : uniforms)
	{
		res += "uniform " + uni.second.type + " " + uni.second.name + ";\n";
	}

	res += "\nuint gid = gl_GlobalInvocationID.x;\n\n";

	return res;
}