#ifndef SPRITE_COMPONENT_CLASS_H
#define SPRITE_COMPONENT_CLASS_H

#include "StaticMeshComponent.h"
#include "TextureAtlas.h"

class SpriteComponent : public StaticMeshComponent {
public:
	TextureAtlas* atlas = nullptr;
	const char* regionName = "_ALL_";
	SpriteComponent();
	SpriteComponent(GameLevel* level, TextureAtlas* _atlas = nullptr, const char* _regionName = "_ALL_");
	void draw(Camera& camera, int shaderSlot) override;
	glm::mat4 worldMatrix() override;
};

#endif