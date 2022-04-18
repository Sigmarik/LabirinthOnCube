#ifndef TEXTURE_ATLAS_CLASS_H
#define TEXTURE_ATLAS_CLASS_H

#include <map>
#include <glm/glm.hpp>

#include "Shader.h"

#include "Texture.h"

class AtlasRegion {
public:
	glm::vec2 corner = glm::vec2(0.0);
	glm::vec2 size = glm::vec2(1.0);
	AtlasRegion(glm::vec2 _corner = glm::vec2(0.0), glm::vec2 _size = glm::vec2(1.0));
};

class TextureAtlas : public Texture {
public:
	std::map<const char*, AtlasRegion> regions = { {"_ALL_", AtlasRegion()} };
	//using Texture::Texture;
	TextureAtlas();
	TextureAtlas(const char* fname, GLuint _slot = GL_TEXTURE0, GLenum format = GL_RGB);
	void defineRegion(const char* name, AtlasRegion region);
	void bindRegion(const char* name, Shader* shader, const char* uniformName = "atlas");
};

#endif