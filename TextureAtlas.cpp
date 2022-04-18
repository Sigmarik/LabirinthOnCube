#include "TextureAtlas.h"

AtlasRegion::AtlasRegion(glm::vec2 _corner, glm::vec2 _size) {
	corner = _corner;
	size = _size;
}

TextureAtlas::TextureAtlas() {
	
}

TextureAtlas::TextureAtlas(const char* fname, GLuint _slot, GLenum format) {
	Texture::Texture(fname, _slot, format);
}

void TextureAtlas::defineRegion(const char* name, AtlasRegion region) {
	regions[name] = region;
}

void TextureAtlas::bindRegion(const char* name, Shader* shader, const char* uniformName) {
	//return;
	//glActiveTexture(slot);
	//Texture::bind();
	AtlasRegion region = regions[name];
	shader->activate();
	glUniform1i(shader->uniform(uniformName), 3);
	glUniform2f(shader->uniform("atlasRegionCorner"), region.corner.x, region.corner.y);
	glUniform2f(shader->uniform("atlasRegionSize"), region.size.x, region.size.y);
}