#ifndef SPRITE_COMPONENT_CLASS_H
#define SPRITE_COMPONENT_CLASS_H

#include "StaticMeshComponent.h"
#include "TextureAtlas.h"

class StaringComponent : public GameComponent {
public:
	StaringComponent();
	StaringComponent(GameLevel* level);
	glm::mat4 worldMatrix() override;
};

#endif