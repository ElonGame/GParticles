#pragma once
#include "AbstractProgram.h"
#include <GL\glew.h>
#include "Model.h"

class RendererProgram : public AbstractProgram
{
public:
	RendererProgram();
	RendererProgram(const GLuint ph, const GLuint mp, const resHeader &a,
		const std::vector<std::string> &u, const std::string &psystemName,
		const GLuint iStep, const GLuint v, const GLuint t, const Model &m,
		const std::string &rType)
		: AbstractProgram(ph, mp, a, u, psystemName, iStep), vao(v), texture(t),
		geoModel(m), renderType(rType)
	{
	};
	~RendererProgram();

	void execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
	void printContents();

private:
	GLuint vao;
	GLuint texture;
	Model geoModel;
	std::string renderType;

	void bindResources();
};