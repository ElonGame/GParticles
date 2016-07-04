#pragma once
#include <GL\glew.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
#include "Utils.h"
#include "Model.h"
#include "GP_Data.h"

class RendererProgram
{
public:
	RendererProgram(const GLuint ph, const resHeader &a,
					const GLuint vao, const GLuint tex, const std::vector<std::string> &u, std::string rt,
					Model &m, GLuint &mp)
		: programhandle(ph), atomics(a), vao(vao), texture(tex), uniforms(u), renderType(rt), model(m), maxParticles(mp) {};
	RendererProgram();
	~RendererProgram();

	void execute(glm::mat4 &modelMat, glm::mat4 &viewMat, glm::mat4 &projectionMat);
	void printContents();
private:
	GLuint programhandle;
	GLuint maxParticles;
	resHeader atomics;
	std::vector<std::string> uniforms;
	GLuint vao;
	GLuint texture;
	Model model;
	std::string renderType;
	
	void bindResources();
};