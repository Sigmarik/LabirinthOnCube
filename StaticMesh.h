#ifndef LABGAME_STATICMESH_CLASS_H
#define LABGAME_STATICMESH_CLASS_H

#define RENDER_MAIN_PASS 0
#define RENDER_DEPTH 1
#define RENDER_POSTPROCESS 2
#define RENDER_INDICES 3

#include <glad/glad.h>
#include <map>

#include "EBO.h"
#include "VBO.h"
#include "VAO.h"

#include "Camera.h"
#include "Shader.h"

#include "tform.h"

class StaticMesh {
public:
	VBO vbo;
	VAO vao;
	EBO ebo;
	std::vector<GLuint> faces;
	std::vector<Vertex> vertices;
	Transform transform = glm::mat4(1.0);
	bool useWorldRotation = false;
	std::map<int, Shader*> shaders = {
		{RENDER_MAIN_PASS,   nullptr},
		{RENDER_DEPTH,       nullptr},
		{RENDER_POSTPROCESS, nullptr}
	};
	StaticMesh();
	StaticMesh(const char* fname, float size = 1.0, bool makeSharp = true);
	void loadFile(const char* fname, float size = 1.0);
	void updateBuffers();
	void clear();
	void recalculateNormals();
	void recalculateTangents();
	void toSharp();
	void toSmooth(float eps = 0.0001);
	void optimize();
	void draw(Camera& camera, int shaderSlot = RENDER_MAIN_PASS, Transform transform = glm::mat4(1.0));
	void attach(StaticMesh* mesh, Transform meshTransform = Transform());
	void assignInt(int value);
	void destroy();
};

#endif