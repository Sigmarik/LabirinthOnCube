#include "SpriteComponent.h"

//  Im laughing red parrot: ZiuZiuZiuZiu  Phuboohhhhh!

// The f*ck I was doing here in comments?

SpriteComponent::SpriteComponent() {}

SpriteComponent::SpriteComponent(GameLevel* level, TextureAtlas* _atlas, const char* _regionName) {
	parentLevel = level;
	atlas = _atlas;
	regionName = _regionName;
	mesh = parentLevel->getMesh("ZSquareMesh.txt");
	//mesh->shaders[RENDER_MAIN_PASS] = parentLevel->getShader("TexturedShader");
	//mesh->shaders[RENDER_DEPTH] = parentLevel->getShader("TexturedDepthShader");
}

void SpriteComponent::draw(Camera& camera, int shaderSlot) {
	//std::cout << "Drawing sprite!" << std::endl;
	if (hidden || !visible) return;
	if (mesh->shaders.find(shaderSlot) == mesh->shaders.end() || !mesh->shaders[shaderSlot]) return;
	transform = glm::mat4(
		glm::vec4(parentLevel->mainCamera->localRight(), 0.0),
		glm::vec4(parentLevel->mainCamera->localUp(), 0.0),
		glm::vec4(parentLevel->mainCamera->forward, 0.0),
		transform[3]
	);
	//std::cout << transform << '\n';
	atlas->bindRegion(regionName, mesh->shaders[shaderSlot]);
	//return;
	StaticMeshComponent::draw(camera, shaderSlot);
	//atlas->unbind();
}

glm::mat4 SpriteComponent::worldMatrix() {
	return glm::mat4(
		transform[0] * localScale,
		transform[1] * localScale,
		transform[2] * localScale,
		GameComponent::worldMatrix()[3]
	);
}