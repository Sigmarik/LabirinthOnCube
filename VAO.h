#ifndef LABGAME_VAO_CLASS_H
#define LABGAME_VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO {
public:
	GLuint id;
	VAO();
	void assignData(VBO& vbo, GLuint layout, GLuint size, GLsizei stride, void* pointer);
	void bind();
	void unbind();
	void destroy();
};

#endif