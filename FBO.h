#ifndef LABGAME_FBO_CLASS_H
#define LABGAME_FBO_CLASS_H

#include <glad/glad.h>
#include <iostream>

class FBO {
public:
	GLuint id;
	GLuint textureID;
	FBO(int width, int height, GLenum attachment=GL_COLOR_ATTACHMENT0, GLenum format=GL_RGBA);
	void bind();
	void unbind();
	void destroy();
};

#endif