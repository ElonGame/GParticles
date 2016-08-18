#include "GP_Loader.h"

bufferUmap GP_Loader::globalBufferInfo;
atomicUmap GP_Loader::globalAtomicInfo;
uniformUmap GP_Loader::globalUniformInfo;
std::vector<GP_Buffer> GP_Loader::reservedBufferInfo;
std::vector<GP_Atomic> GP_Loader::reservedAtomicInfo;
std::vector<GP_Uniform> GP_Loader::reservedUniformInfo;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadProject(std::string filePath)
{
	// get tinyxml handle for project file
	TiXmlDocument doc(filePath);
	if (!doc.LoadFile())
	{
		Utils::exitMessage("Fatal Error",
						   "Failed to load file " + filePath + "! Exiting...");
	}

	TiXmlHandle docH(&doc);
	TiXmlHandle projectH = docH.FirstChild("project");
	if (!projectH.Element())
	{
		Utils::exitMessage("Invalid Input",
						   "Input file must have an outer project tag");
	}


	// load global resources and store reserved resources info
	loadResources(projectH.FirstChild("resources"));
	 

	// iterate through and load particle systems
	TiXmlElement* psystemE = projectH.FirstChildElement("psystem").ToElement();
	for (; psystemE; psystemE = psystemE->NextSiblingElement("psystem"))
	{
		GPARTICLES.pSystems.push_back(loadParticleSystem(psystemE));
		GPARTICLES.pSystems.back().printContents();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadResources(TiXmlHandle resourcesH)
{
	auto lResources = [](TiXmlHandle globalResH, TiXmlHandle reservedResH)
	{	// load global project resources
		loadGlobalBuffers(globalResH);
		loadGlobalAtomics(globalResH);
		loadGlobalUniforms(globalResH);
		
		// collect reserved resource default info
		collectReservedResourceInfo(reservedResH);
	};

	// start by processing current file
	lResources(resourcesH.FirstChild("global"), resourcesH.FirstChild("instance"));


	// if there is a resources prefab continue loading/collecting resources
	std::string prefabPath;
	resourcesH.ToElement()->QueryStringAttribute("prefab", &prefabPath);

	TiXmlDocument doc(prefabPath);
	if (doc.LoadFile())
	{
		TiXmlHandle docH(&doc);
		TiXmlHandle prefabH = docH.FirstChild("resources");
		
		lResources(prefabH.FirstChild("global"), prefabH.FirstChild("instance"));
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

		if (GPDATA.getBuffer(b.name, b))
		{
			continue;
		}

		// parse buffer info, initialize and store it
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryUnsignedAttribute("elements", &b.elements);},
						bufferE, "Must provide buffer attribute with number of elements");
		queryAttribute(	[&b](TiXmlElement* e) {return e->QueryStringAttribute("type", &b.type);},
						bufferE, "Must provide buffer attribute with a type");
		
		b.init();

		globalBufferInfo.emplace(b.name, b);
		GPDATA.addBuffer(b);

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
		
		if (GPDATA.getAtomic(a.name, a))
		{
			continue;
		}

		// parse atomic info, initialize and store it
		queryAttribute(	[&a](TiXmlElement* e) {return e->QueryUnsignedAttribute("value", &a.resetValue);},
						atomicE, "Must provide atomic attribute with a value > 0");

		a.init();

		globalAtomicInfo.emplace(a.name, a);
		GPDATA.addAtomic(a);

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

		if (GPDATA.getUniform(u.name, u))
		{
			continue;
		}

		// parse atomic info, initialize and store it
		queryAttribute(	[&u](TiXmlElement* e) {return e->QueryStringAttribute("type", &u.type);},
						uniformE, "Must provide uniform attribute with a type");
		queryUniformValue(uniformE, u);

		globalUniformInfo.emplace(u.name, u);
		GPDATA.addUniform(u);

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

		for (size_t i = 0; i < reservedBufferInfo.size() && !skipIter; i++)
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

		for (size_t i = 0; i < reservedAtomicInfo.size() && !skipIter; i++)
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
		for (size_t i = 0; i < reservedUniformInfo.size() && !skipIter; i++)
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
void GP_Loader::collectColliders(reservedResources &rr, TiXmlElement* collidersE)
{
	if (!collidersE)
	{
		return;
	}

	// collect static colliders
	std::string type;
	glm::vec4 value;
	TiXmlElement* colliderE = collidersE->FirstChildElement("static");
	for (; colliderE; colliderE = colliderE->NextSiblingElement("static"))
	{
		queryAttribute(	[&](TiXmlElement* e) {return e->QueryStringAttribute("type", &type);},
						colliderE, "Must provide static tag with a type attribute");

		colliderE->QueryFloatAttribute("x", &value.x);
		colliderE->QueryFloatAttribute("y", &value.y);
		colliderE->QueryFloatAttribute("z", &value.z);

		if (type == "sphere")
		{
			colliderE->QueryFloatAttribute("r", &value.w);
			if (value.w <= 0)
			{
				Utils::exitMessage("Invalid Input", "Sphere radius must be > 0");
			}

			rr.spheres.push_back(value);
		}
		else if (type == "plane")
		{
			colliderE->QueryFloatAttribute("d", &value.w);
			rr.planes.push_back(value);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadProperties(TiXmlElement* propertiesE, psProperties &psp, loadFunctionInfo lfi)
{
	// model matrix
	glm::vec3 pos = glm::vec3();
	TiXmlElement* transformE = propertiesE->FirstChildElement("position");
	if (transformE)
	{
		transformE->QueryFloatAttribute("x", &pos.x);
		transformE->QueryFloatAttribute("y", &pos.y);
		transformE->QueryFloatAttribute("z", &pos.z);
	}

	float angle = 0;
	glm::vec3 rot = glm::vec3(0,1,0);
	transformE = propertiesE->FirstChildElement("rotation");
	if (transformE)
	{
		transformE->QueryFloatAttribute("x", &rot.x);
		transformE->QueryFloatAttribute("y", &rot.y);
		transformE->QueryFloatAttribute("z", &rot.z);
		transformE->QueryFloatAttribute("angle", &angle);
	}

	glm::vec3 scale = glm::vec3(1);
	transformE = propertiesE->FirstChildElement("scale");
	if (transformE)
	{
		transformE->QueryFloatAttribute("x", &scale.x);
		transformE->QueryFloatAttribute("y", &scale.y);
		transformE->QueryFloatAttribute("z", &scale.z);
	}

	// if model matrix has not been set
	if (psp.model == glm::mat4())
	{
		psp.model = glm::translate(psp.model, pos);
		psp.model = glm::rotate(psp.model, glm::radians(angle), rot);
		psp.model = glm::scale(psp.model, scale);
	}
	

	// lifetime
	std::string unit;
	TiXmlElement* lifetimeE = propertiesE->FirstChildElement("lifetime");
	if (lifetimeE && psp.lifetime == 0 && (NO_PREFAB || SECOND_ITERATION))
	{
		queryAttribute([&](TiXmlElement* e) {return e->QueryUnsignedAttribute("value", &psp.lifetime);},
			lifetimeE, "Must provide lifetime tag with a value attribute");

		queryAttribute([&](TiXmlElement* e) {return e->QueryStringAttribute("unit", &unit);},
			lifetimeE, "Must provide lifetime tag with a unit attribute");

		// looping attribute is optional, defaults to true
		lifetimeE->QueryBoolAttribute("looping", &psp.looping);

		if (unit == "seconds")
		{
			psp.lifetime *= 1000;
		}
		else if (unit == "milliseconds");
		else
		{
			Utils::exitMessage("Invalide Input", "Unrecognized time unit. Supported time units are: seconds, milliseconds");
		}
	}


	// numWorkGroups
	TiXmlElement* nWorkGroupE = propertiesE->FirstChildElement("numWorkGroups");
	if (nWorkGroupE)
	{
		nWorkGroupE->QueryUnsignedAttribute("value", &psp.numWorkGroups);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GP_ParticleSystem GP_Loader::loadParticleSystem(TiXmlElement* psystemE)
{
	// load particle system reserved resources
	reservedResources rr;
	loadReservedPSResources(rr, psystemE);
	collectColliders(rr, psystemE->FirstChildElement("colliders"));


	// load psystem properties
	TiXmlElement* propertiesE = psystemE->FirstChildElement("properties");
	if (!propertiesE)
	{
		Utils::exitMessage("Fatal Error", "psystem must have a properties tag");
	}

	psProperties psp;

	std::string prefabPath = "";
	propertiesE->QueryStringAttribute("prefab", &prefabPath);
	
	TiXmlDocument doc(prefabPath);
	if (doc.LoadFile())
	{
		loadProperties(propertiesE, psp, FIRST_ITERATION);

		TiXmlHandle docH(&doc);
		propertiesE = docH.FirstChild("properties").ToElement();

		loadProperties(propertiesE, psp, SECOND_ITERATION);
	}
	else
	{
		loadProperties(propertiesE, psp, NO_PREFAB);
	}


	// load psystem events (emission, update, render)
	TiXmlElement* eventsE = psystemE->FirstChildElement("events");
	if (!eventsE)
	{
		Utils::exitMessage("Fatal Error", "psystem must have an events tag");
	}

	ComputeProgram emission;
	TiXmlElement* eventE = eventsE->FirstChildElement("emission");
	if (!loadComputeProgram(psp.numWorkGroups, rr, eventE, emission))
	{
		Utils::exitMessage("Fatal Error", "Unable to load emission event");
	}

	ComputeProgram update;
	eventE = eventsE->FirstChildElement("update");
	if (!loadComputeProgram(psp.numWorkGroups, rr, eventE, update))
	{
		Utils::exitMessage("Fatal Error", "Unable to load update event");
	}

	RendererProgram render;
	eventE = eventsE->FirstChildElement("render");
	if (!loadRenderProgram(rr, eventE, render))
	{
		Utils::exitMessage("Fatal Error", "Unable to load render event");
	}



	return GP_ParticleSystem(	emission, update, render, psp.model,
						psp.numWorkGroups, psp.lifetime, psp.looping);
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
		GPDATA.addUniform(u);
	}

	for (auto resAtmInfo : reservedAtomicInfo)
	{
		GP_Atomic a = resAtmInfo;

		std::string originalname = a.name;
		a.name = psystemName + "_" + a.name;

		a.init();

		rr.reservedAtomics.emplace(originalname, a);
		GPDATA.addAtomic(a);
	}

	// check and apply resource value overrides
	loadInitialResourceOverrides(rr, psystemE);

	// load default reserved atomics, uniforms and buffers
	if (rr.reservedUniforms.find("maxParticles") == rr.reservedUniforms.end())
	{
		Utils::exitMessage("Fatal error", "maxParticles uniform instance definition required!");
	}

	rr.maxParticles = (GLuint)rr.reservedUniforms.at("maxParticles").value[0].x;
	for (auto resBufInfo : reservedBufferInfo)
	{
		GP_Buffer b = resBufInfo;
		std::string originalName = b.name;
		b.name = psystemName + "_" + b.name;
		
		b.init(rr.maxParticles);

		rr.reservedBuffers.emplace(originalName, b);
		GPDATA.addBuffer(b);
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
		queryAttribute(	[&](TiXmlElement* e) {return e->QueryStringAttribute("name", &resName);},
						resE, "Must provide override tag with a name attribute");

		queryAttribute(	[&](TiXmlElement* e) {return e->QueryStringAttribute("type", &resType);},
						resE, "Must provide override tag with a type attribute");

		// TODO: check res type
		if (resType == "uniform" && rr.reservedUniforms.find(resName) != rr.reservedUniforms.end())
		{
			queryUniformValue(resE, rr.reservedUniforms.at(resName));
			GPDATA.addUniform(rr.reservedUniforms.at(resName));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::loadIterationResourceOverrides(
	TiXmlElement * programE, atomicUmap &aum, bufferUmap &bum, uniformUmap &uum)
{
	if (programE == nullptr)
	{
		return;
	}

	TiXmlElement* resE = programE->FirstChildElement("override");
	
	if (resE == nullptr)
	{
		return;
	}

	std::string resType, resName, resValue;
	for (; resE; resE = resE->NextSiblingElement("override"))
	{
		resE->QueryStringAttribute("type", &resType);
		resE->QueryStringAttribute("name", &resName);
		resE->QueryStringAttribute("value", &resValue);

		if (resType == "atomic")
		{
			aum.at(resName).reset = true;
			aum.at(resName).resetValue = std::stoul(resValue, nullptr, 0);
			GPDATA.addAtomic(aum.at(resName));
		}
		else if (resType == "uniform")
		{ 
			uum.at(resName).value[0].x = std::stof(resValue);
			GPDATA.addUniform(uum.at(resName));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Loader::loadComputeProgram(GLuint numWorkGroups, reservedResources &rr, TiXmlElement* eventE, ComputeProgram &cp)
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


	GLuint iterationStep = 0;
	std::vector<std::string> fPaths;

	auto getComputeInfo = [&](TiXmlElement* e)
	{	// and mark the indicated resources to be reset every iteration
		loadIterationResourceOverrides(e, cpAtomics, cpBuffers, cpUniforms);

		// parse file paths that compose final shader
		if (fPaths.empty()) collectPaths(e, "file", fPaths);

		e->QueryUnsignedAttribute("iterationStep", &iterationStep);

	};
	getComputeInfo(eventE);

	// if there is an event prefab continue loading/collecting resources
	std::string prefabPath;
	eventE->QueryStringAttribute("prefab", &prefabPath);

	TiXmlDocument doc(prefabPath);
	if (doc.LoadFile())
	{
		TiXmlHandle docH(&doc);
		getComputeInfo(docH.FirstChild(eventE->ValueStr()).ToElement());
	}


	// create shader program
	GLuint cpHandle = glCreateProgram();

	// shader source = resource header + files source
	std::string psystemName;
	eventE->Parent()->Parent()->ToElement()->QueryStringAttribute("name", &psystemName);

	std::string shaderSource = generateComputeHeader(cpBuffers, cpAtomics, cpUniforms, rr.spheres, rr.planes);
	shaderSource += createFinalShaderSource(fPaths, psystemName);


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

	cp = ComputeProgram(cpHandle, rr.maxParticles, aHeaders, uHeaders, psystemName, iterationStep, numWorkGroups, bHeaders);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::getRenderInfo(renderInfo &rl, TiXmlElement* eventE)
{
	if (eventE == NULL)
		return;
	
	// renderType
	TiXmlElement* renderTypeE = eventE->FirstChildElement("rendertype");
	if (renderTypeE && rl.rendertype.empty())
	{
		renderTypeE->QueryStringAttribute("type", &rl.rendertype);

		if (rl.rendertype == "billboard" || rl.rendertype == "model")
		{
			queryAttribute([&](TiXmlElement* e) {return e->QueryStringAttribute("path", &rl.path);},
				renderTypeE, "Must provide rendertype with a resource path");
		}
	}

	// collect iterationStep
	eventE->QueryUnsignedAttribute("iterationStep", &rl.iterationStep);

	// collect shader file paths
	collectPaths(eventE, "vertfile", rl.vsPath);
	collectPaths(eventE, "geomfile", rl.gmPath);
	collectPaths(eventE, "fragfile", rl.fgPath);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GP_Loader::collectPaths(TiXmlElement* elem, const char *tag, std::vector<std::string> &target)
{
	if (!target.empty())
		return;

	TiXmlElement* pathE = elem->FirstChildElement(tag);
	if (pathE == NULL)
		return;
	
	std::string pathStr;
	for (; pathE; pathE = pathE->NextSiblingElement(tag))
	{
		pathE->QueryStringAttribute("path", &pathStr);
		target.push_back(pathStr);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GP_Loader::loadRenderProgram(reservedResources &rr, TiXmlElement* eventE, RendererProgram &rp)
{
	// fill renderInfo struct with the final info to be processed
	// starting with the original file
	renderInfo rl;
	getRenderInfo(rl, eventE);

	// check if there is a prefab and continue filling the the struct
	std::string prefabPath;
	eventE->QueryStringAttribute("prefab", &prefabPath);
	
	TiXmlDocument doc(prefabPath);
	if (doc.LoadFile())
	{
		TiXmlHandle docHandle(&doc);
		TiXmlElement* prefabE = docHandle.FirstChild("prefab").ToElement();
		getRenderInfo(rl, prefabE);
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

	std::string psystemName;
	eventE->Parent()->Parent()->ToElement()->QueryStringAttribute("name", &psystemName);
	auto loadAndAttach = [&](auto type, auto paths, auto in, auto out)
	{
		std::string shaderSource = generateRenderHeader(rendBuffers,
								   rendAtomics, rendUniforms, in, out, rl.rendertype);
		shaderSource += createFinalShaderSource(paths, psystemName);
		Shader shader(type, shaderSource, psystemName + "_" + eventE->ValueStr());
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
		GLint posLocation = glGetAttribLocation(rpHandle, "vertexPosition");
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)0);

		// vertex Normals
		glEnableVertexAttribArray(1);
		GLint norLocation = glGetAttribLocation(rpHandle, "vertexNormal");
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

	rp = RendererProgram(rpHandle, rr.maxParticles, aHeaders, uHeaders, psystemName, rl.iterationStep, vao, texture.getId(), model, rl.rendertype);

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
std::string GP_Loader::createFinalShaderSource(std::vector<std::string> fPaths,
											   std::string psystemName)
{
	// parse file fragments into string
	std::string shaderString = "";
	for (auto fPath : fPaths)
	{
		shaderString += "\n\n" + fillTemplate(fPath, psystemName);
	}

	return shaderString;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GP_Loader::generateRenderHeader(bufferUmap &buffers, atomicUmap &atomics,
	uniformUmap &uniforms, std::string in, std::string out, std::string renderType)
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
	res += "\n" + (out != "") ? (bufferOuts) : "";

	if (renderType == "model")
	{
		res += "\nin vec3 vertexPosition" + in + ";\nin vec3 vertexNormal" + in + ";\n\n"
			   "out vec3 vertexPosition" + out + ";\nout vec3 vertexNormal" + out + ";\n\n";
	}

	res += "uniform mat4 model, view, projection;\n\n";

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
	}

	return "\n\n" + mainStr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GP_Loader::generateComputeHeader(bufferUmap &buffers, atomicUmap &atomics, uniformUmap &uniforms,
											 std::vector<glm::vec4> spheres, std::vector<glm::vec4> planes)
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
	res += "uniform mat4 model, view, projection;\n\n";

	// uniforms
	for (auto &uni : uniforms)
	{
		res += "uniform " + uni.second.type + " " + uni.second.name + ";\n";
	}

	// world colliders
	//res += "\n\nvec4 sphere, plane;\nvec3 collisionPoint, normal;\n";

	// spheres
	if (!spheres.empty())
	{
		res +=	"\n\nconst uint MAX_SPHERES = " + std::to_string(spheres.size())
				+ ";\nconst vec4 spheres[MAX_SPHERES] =\n{";
		for (size_t i = 0; i < spheres.size() - 1; i++)
		{
			auto s = spheres[i];

			res +=	"\n\tvec4(" + std::to_string(s.x) + ", " + std::to_string(s.y)
					+ ", " + std::to_string(s.z) + ", " + std::to_string(s.w) + "),";
		}

		auto s = spheres[spheres.size() - 1];
		res +=	"\n\tvec4(" + std::to_string(s.x) + ", " + std::to_string(s.y) + ", "
				+ std::to_string(s.z) + ", " + std::to_string(s.w) + ")\n};";
	}
	else
	{
		res += "\n\nconst uint MAX_SPHERES = 0;\nconst vec4 spheres[1] = {vec4(0)};";
	}

	// planes
	if (!planes.empty())
	{
		res +=	"\n\nconst uint MAX_PLANES = " + std::to_string(planes.size())
				+ ";\nconst vec4 planes[MAX_PLANES] =\n{";
		for (size_t i = 0; i < planes.size() - 1; i++)
		{
			auto p = planes[i];

			res +=	"\n\tvec4(" + std::to_string(p.x) + ", " + std::to_string(p.y)
					+ ", " + std::to_string(p.z) + ", " + std::to_string(p.w) + "),";
		}

		auto p = planes[planes.size() - 1];
		res += "\n\tvec4(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ", "
			+ std::to_string(p.z) + ", " + std::to_string(p.w) + ")\n};";
	}
	else
	{
		res += "\n\nconst uint MAX_PLANES = 0;\nconst vec4 planes[1] = {vec4(0)};";
	}

	res += "\n\n\nconst uint gid = gl_GlobalInvocationID.x;\n\n";

	return res;
}