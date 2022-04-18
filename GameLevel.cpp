#include "GameLevel.h"

GameLevel::GameLevel() {}

void GameLevel::loadAsset(const char* fname, bool makeSharp, float size) {
	std::cout << "Loading asset " << fname << '\n';
	worldMeshes[fname] = StaticMesh(fname, size, makeSharp);
}

void GameLevel::loadShader(const char* name, const char* vshName, const char* fshName) {
	std::cout << "Loading shader " << name << '\n';
	renderShaders[name] = Shader(vshName, fshName);
}

void GameLevel::loadAtlas(const char* fname, GLuint slot, GLenum format) {
	std::cout << "Loading atlas " << fname << '\n';
	atlases[fname] = TextureAtlas(fname, slot, format);
}

StaticMesh* GameLevel::getMesh(const char* fname) {
	if (worldMeshes.find(fname) == worldMeshes.end())
		throw std::runtime_error::runtime_error(std::string(fname) + " was not found");
	return &worldMeshes[fname];
}

Shader* GameLevel::getShader(const char* name) {
	if (renderShaders.find(name) == renderShaders.end()) 
		throw std::runtime_error::runtime_error(std::string(name) + " was not found");
	return &renderShaders[name];
}

TextureAtlas* GameLevel::getAtlas(const char* name) {
	if (atlases.find(name) == atlases.end())
		throw std::runtime_error::runtime_error(std::string(name) + " was not found");
	return &atlases[name];
}

GameComponent::GameComponent() {}

glm::mat4 GameComponent::worldMatrix() {
	if (!parent) return transform;
	if (!useWorldRotation) return parent->worldMatrix() * transform;
	glm::mat4 rawMatrix = parent->worldMatrix() * transform;
	return glm::mat4(
		glm::vec4(glm::vec3(transform[0]), 0.0),
		glm::vec4(glm::vec3(transform[1]), 0.0),
		glm::vec4(glm::vec3(transform[2]), 0.0),
		glm::vec4(glm::vec3(rawMatrix[3]), 1.0)
	);
}

void GameComponent::attach(GameComponent* component) {
	children.push_back(component);
	component->parent = this;
}

void GameComponent::attachTo(GameComponent* component) {
	component->attach(this);
}

void GameComponent::draw(Camera& camera, int shaderSlot) {
	// Don't make any draw calls if invisible.
	if (!visible) return;
	for (GameComponent* component : children) {
		component->draw(camera, shaderSlot);
	}
	//std::cout << "Called draw on " << children.size() << " components\n";
}

void GameComponent::update(float deltaTime) {
	for (GameComponent* component : children) {
		if (component) component->update(deltaTime);
	}
}

bool GameComponent::getVisible() {
	if (!parent) return visible;
	return parent->getVisible() && visible;
}

void GameComponent::assignParent() {
	for (GameComponent* component : children) {
		component->parent = this;
		component->assignParent();
	}
}