#ifndef LABGAME_EBO_CLASS_H
#define LABGAME_EBO_CLASS_H

#include <glad/glad.h>
#include <vector>

class EBO {
public:
	GLuint id;
	EBO();
	EBO(std::vector<GLuint>& indices);
	void bind();
	void unbind();
	void destroy();
};

#endif