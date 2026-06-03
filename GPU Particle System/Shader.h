#pragma once

#include<stdio.h>
#include<string>
#include<iostream>
#include<fstream>

#include<glad/glad.h>

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);

	void Validate();

	std::string ReadFile(const char* FileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();

	GLuint GetEyePositionLocation();

	void UseShader();
	void ClearShader();

	~Shader();
private:

	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition;

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode);
	void AddShader(GLuint theprogram, const char* shaderCode, GLenum shaderType);

	void CompileProgram();
};

