#pragma once
#include <glew\glew.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
#include "Utils.h"
#include "Camera.h"
#include "Model.h"

class RendererProgram
{
public:
	RendererProgram(const GLuint ph, const std::vector<std::pair<GLuint, GLuint>> &ah,
					const GLuint vao, const GLuint tex, const uniformUmap &u, std::string rt,
					Model &m)
		: programhandle(ph), atomicHandles(ah), vao(vao), texture(tex), uniforms(u), renderType(rt), model(m) {};
	RendererProgram();
	~RendererProgram();

	void execute(glm::mat4 &modelMat, Camera cam);
	void printContents();
private:
	GLuint programhandle;
	std::vector<std::pair<GLuint, GLuint>> atomicHandles;
	uniformUmap uniforms;
	GLuint vao;
	GLuint texture;
	Model model;
	std::string renderType;
	
	void bindResources();
	void setUniforms();
};