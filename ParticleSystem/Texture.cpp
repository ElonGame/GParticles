#include "Texture.h"



Texture::Texture()
{
	initTexture();
}

Texture::Texture(std::string path, std::string typeName)
{
	initTexture();
	this->typeName = typeName;
	setImage(path);
}

Texture::~Texture()
{
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setImage(std::string path)
{
	unsigned int im;
	unsigned char *imageData;
	ilGenImages(1, &im);
	ilBindImage(im);
	ilLoadImage((ILstring)path.c_str());
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	imageData = ilGetData();

	this->bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	this->unbind();

	ilBindImage(0);
	ilDeleteImages(1, &im);
}

unsigned int Texture::getId()
{
	return id;
}

unsigned int Texture::getWidth()
{
	return width;
}

unsigned int Texture::getHeight()
{
	return height;
}

std::string Texture::getTypeName()
{
	return typeName;
}

void Texture::initTexture()
{
	glGenTextures(1, &id);
	width = 0;
	height = 0;

	this->bind();

	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->unbind();
}