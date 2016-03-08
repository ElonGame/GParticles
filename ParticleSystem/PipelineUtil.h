#pragma once
#include <glew\glew.h>
#include <stdio.h>
#include <string>
#include <fstream>

void initGL();
bool compileShaderFiles(GLuint shaderID, std::string filePath);
bool linkProgram(GLuint programHandle);
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);