#ifndef LABGAME_TEXTURE_CLASS_H
#define LABGAME_TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

class Texture {
public:
	GLuint id;
	GLuint slot = GL_TEXTURE0;
	Texture();
	Texture(const char* fname, GLuint _slot = GL_TEXTURE0, GLenum format=GL_RGB);
	void bind();
	void unbind();
	void destroy();
};

#endif