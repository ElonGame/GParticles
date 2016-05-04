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
	RendererProgram(const GLuint ph, const std::vector<std::pair<GLuint, GLuint>> &ah, const GLuint vao, const GLuint tex, const uniformUmap &u)
		: programhandle(ph), atomicHandles(ah), vao(vao), texture(tex), uniforms(u) {};
	RendererProgram();
	~RendererProgram();

	void execute(glm::mat4 &model, Camera cam);
	void printContents();
private:
	GLuint programhandle;
	std::vector<std::pair<GLuint, GLuint>> atomicHandles;
	uniformUmap uniforms;
	GLuint vao;
	GLuint texture;

	void bindResources();
	void setUniforms();
};