#include "StaticMeshComponent.h"

void StaticMeshComponent::draw(Camera& camera, int shaderSlot) {
	if (visible && !hidden) {
		//std::cout << "Mesh draw is called with slot " << shaderSlot << '\n';
		if (mesh->shaders[shaderSlot]) {
			mesh->shaders[shaderSlot]->activate();
			glUniform1i(mesh->shaders[shaderSlot]->uniform("shouldWave"), shouldWave);
		}
		mesh->draw(camera, shaderSlot, worldMatrix());
	}
	GameComponent::draw(camera, shaderSlot);
}