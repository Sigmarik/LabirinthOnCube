#include "StaringComponent.h"

StaringComponent::StaringComponent() {}

StaringComponent::StaringComponent(GameLevel* level) {
	parentLevel = level;
}

glm::mat4 StaringComponent::worldMatrix() {
	return glm::mat4(
		glm::vec4(parentLevel->mainCamera->localRight() * localScale, 0.0),
		glm::vec4(parentLevel->mainCamera->localUp() * localScale, 0.0),
		glm::vec4(parentLevel->mainCamera->forward * localScale, 0.0),
		GameComponent::worldMatrix()[3]
	);
}