#pragma once
#include "AbstractStage.h"
#include "Model.h"

class RenderStage : public AbstractStage
{
public:
	RenderStage();
	RenderStage(const GLuint ph, const GLuint mp, const resHeader &a,
		const std::vector<std::string> &u, const std::string &psystemName,
		const std::set<std::string> &tgs, const GLuint iStep, const GLuint v,
		const GLuint t, const Model &m, const std::string &rType)
		: AbstractStage(ph, mp, a, u, psystemName, tgs, iStep), vao(v),
		texture(t), geoModel(m), renderType(rType)
	{
	};
	~RenderStage();

	void execute(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
	void printContents();

private:
	GLuint vao;
	GLuint texture;
	Model geoModel;
	std::string renderType;

	void bindResources();
};