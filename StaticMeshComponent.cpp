#include "StaticMeshComponent.h"

void StaticMeshComponent::draw(Camera& camera, int shaderSlot) {
	if (visible && !hidden) {
		//std::cout << "Mesh draw is called with slot " << shaderSlot << '\n';
		mesh->draw(camera, shaderSlot, worldMatrix());
	}
	GameComponent::draw(camera, shaderSlot);
}