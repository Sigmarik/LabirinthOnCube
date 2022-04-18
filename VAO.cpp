#include "VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &id);
}

void VAO::assignData(VBO& vbo, GLuint layout, GLuint size, GLsizei stride, void* pointer) {
	glBindVertexArray(id);
	glVertexAttribPointer(layout, size, GL_FLOAT, GL_FALSE, stride, pointer);
	glEnableVertexAttribArray(layout);
	glBindVertexArray(0);
}

void VAO::bind() {
	glBindVertexArray(id);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::destroy() {
	glDeleteVertexArrays(1, &id);
}