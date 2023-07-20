#pragma once
#ifndef SHADER_H
#define SHADER_H

#include<GL/glew.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{

public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();

	void setInt(const std::string& name, int value)const;

	void setBool(const std::string& name, bool value)const;

	void setFloat(const std::string& name, float value)const;

	void setMat4(const std::string& name, glm::mat4 & value  )const;

	~Shader();
};

#endif // !shader.h
