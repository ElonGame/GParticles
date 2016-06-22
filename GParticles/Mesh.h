#pragma once
#include <vector>
#include "Utils.h"
#include "Texture.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> v, std::vector<GLuint> i, std::vector<Texture> t)
		: vertices(v), indices(i), textures(t)
	{
		this->setupMesh();
	};
	~Mesh();

	void draw();

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

private:
	void setupMesh();

	GLuint vao, vbo, ebo;
};

