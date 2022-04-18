#ifndef LABGAME_VBO_CLASS_H
#define LABGAME_VBO_CLASS_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

class Vertex {
public:
	glm::vec3 position = glm::vec3(0.0);
	glm::vec3 normal = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 tangent = glm::vec3(1.0, 0.0, 0.0);
	glm::vec2 uv = glm::vec2(0.0);
	glm::vec4 color = glm::vec4(0.0, 0.0, 0.0, 1.0);
	GLint integer = 0;
	Vertex();
	Vertex(glm::vec3 pos, glm::vec2 _uv, glm::vec4 _color = glm::vec4(0.0, 0.0, 0.0, 1.0));
};

class VBO {
public:
	GLuint id;
	VBO();
	VBO(std::vector<Vertex>& vertices);
	void bind();
	void unbind();
	void destroy();
};

#endif