#ifndef LABGAME_SHADER_H
#define LABGAME_SHADER_H

#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

class Shader {
private:
	void compileErrors(unsigned int shader, const char* type);
public:
	GLuint id;
	Shader();
	Shader(const char* vertexName, const char* fragmentName);
	void activate();
	void destroy();
	GLint uniform(const char* name);
};

#endif