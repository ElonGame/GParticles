#pragma once
#include <IL\il.h>
#pragma comment(lib,"devil.lib")
#include <GL\glew.h>
#include <string>

class Texture
{
public:
	Texture();
	Texture(std::string path, std::string typeName = "billboard");
	~Texture();

	void bind();
	void unbind();
	void setImage(std::string path);
	unsigned int getId();
	unsigned int getWidth();
	unsigned int getHeight();
	std::string getTypeName();

private:
	unsigned int id;
	unsigned int width;
	unsigned int height;
	std::string typeName;

	void initTexture();
};

