#pragma once
#include <glew\glew.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
#include "Utils.h"
#include "Camera.h"

class RendererProgram
{
public:
	RendererProgram(GLuint ph, const std::vector<GLuint> &ah, const GLuint &vao, const uniformUmap &u)
		: programhandle(ph), atomicHandles(ah), vao(vao), uniforms(u) {};
	RendererProgram();
	~RendererProgram();

	void execute(glm::mat4 &model, Camera cam);
	void printContents();
private:
	GLuint programhandle;
	std::vector<GLuint> atomicHandles;
	uniformUmap uniforms;
	GLuint vao;

	void bindResources();
	void setUniforms();
};